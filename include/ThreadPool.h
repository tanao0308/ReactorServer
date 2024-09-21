#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <stdexcept>
#include <memory>

/*
TODO 加入核心线程数、最大线程数和线程创建删除策略
*/
struct WorkerThread {
	int priority;
	std::thread work_thread;
};
class ThreadPool {
  public:
	static ThreadPool& getInstance(size_t core_thread_num, size_t max_thread_num, size_t max_queue_length) {
		std::cout<<"start thread pool"<<std::endl;
		static ThreadPool instance(core_thread_num, max_thread_num, max_queue_length);
		return instance;
	}
	ThreadPool(ThreadPool const&) = delete;
	void operator=(ThreadPool const&) = delete;
	~ThreadPool() {
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			running = false;
		}
		cond_var.notify_all();
		for(auto &worker : workers) {
			if(worker.work_thread.joinable()) {
				worker.work_thread.join();
			}
		}
	}
	// 提交一个任务到线程池
	template<class F, class... Args>
	auto submit(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
		// 判断是否到达任务队列上限
		if (tasks.size() >= max_queue_length_) {
			// 判断是否可以增加线程数
			if (workers.size() < max_thread_num_) {
				workers.push_back((WorkerThread){(int)workers.size(), std::thread([this] {this->ThreadWorker(); })});
			}
			return;
		}
		// 获取任务返回值的类型
		using return_type = typename std::result_of<F(Args...)>::type;
		// 创建一个共享的可打包任务（packaged_task），该任务将执行函数 f 和参数 args
		auto task = std::make_shared<std::packaged_task<return_type()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);
		// 获取与该任务相关联的未来对象（future），用于获取任务的结果
		std::future<return_type> res = task->get_future();
		// 使用唯一锁（unique_lock）保护任务队列的访问
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			// 检查线程池是否在运行，如果已停止，则抛出异常
			if (!running)
				throw std::runtime_error("enqueue on stopped ThreadPool");
			// 将任务包装为可调用对象并添加到任务队列中
			tasks.emplace([task]() { (*task)(); });
		}
		// 通知一个等待的线程有新的任务可执行
		cond_var.notify_one(); // 改为通知优先级最低的线程
		// 返回与任务关联的未来对象，用户可以通过它获取任务的结果
		return res;
	}
  private:
	// queue_mutex 用来保护对 tasks 队列和 stop 标志的访问，因为它们通常是在相同的代码块中被访问和修改的。
	bool running; // 需互斥
	std::mutex queue_mutex;
	std::condition_variable cond_var;
	int core_thread_num_;
	int max_thread_num_;
	int max_queue_length_;
	std::vector<WorkerThread> workers;
	std::queue<std::function<void()>> tasks; // 需互斥
	ThreadPool(size_t core_thread_num, size_t max_thread_num, size_t max_queue_length): running(1), core_thread_num_(core_thread_num),
		max_thread_num_(max_thread_num), max_queue_length_(max_queue_length) {
		for(int i=0; i<core_thread_num; ++i) {
			workers.emplace_back((WorkerThread){i, std::thread([this] {this->ThreadWorker(); })});
		}
	}
	void ThreadWorker() {
		while(true) {
			std::function<void()> task;
			{
				// 条件变量基础操作
				std::unique_lock<std::mutex> lock(queue_mutex);
				cond_var.wait(lock, [this]() { return !this->running || !this->tasks.empty(); });

				// 进程池停止且队列中任务都完成了，则进程结束
				if(running == 0 && tasks.empty())
					return;
				
				if(tasks.empty())
					throw std::runtime_error("visit empty tasks queue.");
				task = std::move(tasks.front());
				tasks.pop();
			}
			task();
		}
	}
};

#endif
