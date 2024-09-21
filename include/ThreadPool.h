#ifndef THREAD_POOL_H
#define THREAD_POOL_H

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
class ThreadPool
{
public:
	static ThreadPool& getInstance(size_t thread_num = std::thread::hardware_concurrency()) {
		std::cout<<"start thread pool"<<std::endl;
		static ThreadPool instance(thread_num);
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
		for(std::thread &worker : workers)
			if(worker.joinable())
				worker.join();
	}
	
	template<class F, class... Args>
	auto submit(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
		using return_type = typename std::result_of<F(Args...)>::type;
		auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		std::future<return_type> res = task->get_future();
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			if(!running)
				throw std::runtime_error("enqueue on stopped ThreadPool");
			tasks.emplace([task]() { (*task)(); });
		}
		cond_var.notify_one();
		return res;
	}

private:
	// queue_mutex 用来保护对 tasks 队列和 stop 标志的访问，因为它们通常是在相同的代码块中被访问和修改的。
	bool running; // 需互斥
	std::mutex queue_mutex;
	std::condition_variable cond_var;
	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks; // 需互斥
	
	ThreadPool(size_t thread_num): running(1) {
		for(int i=0; i<thread_num; ++i)
			workers.emplace_back(std::thread([this] {this->ThreadWorker(); } ));
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
