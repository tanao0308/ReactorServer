#ifndef SUB_REACTOR_H
#define SUB_REACTOR_H

#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include <stdexcept>
#include <map>
#include "ThreadPool.h"


using EventCallback = std::function<void()>;

class CallbackManager
{
public:
	CallbackManager(int thread_num): thread_pool(ThreadPool::getInstance(thread_num)) {}
    void add_callback(int fd, EventCallback callback)
	{
        callbacks[fd] = callback;
    }

    void remove_callback(int fd)
	{
        callbacks.erase(fd);
    }

    void execute_callback(int fd)
	{
        if (callbacks.find(fd) != callbacks.end())
            thread_pool.submit(callbacks[fd]());
    }

private:
    std::map<int, EventCallback> callbacks;
	ThreadPool& thread_pool;
};


class SubReactor // todo
{
public:
	SubReactor(int thread_num=5): callback_manager(thread_num)
	{
        epoll_fd = epoll_create1(0);
        if (epoll_fd == -1) 
            throw std::runtime_error("Failed to create epoll file descriptor");
    }

	~SubReactor()
	{
		close(epoll_fd);
	}

	// 子reactor的主进程
	void start()
	{
		while (true) 
        {
            epoll_event events[MAX_EVENTS];
            int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
            if (event_count == -1) 
                throw std::runtime_error("Epoll wait error");
            for (int i=0; i<event_count; ++i) 
                handle_event(events[i].data.fd);
        }
	}

	// 接收主reactor分配的客户端套接字
	void handle_client(int client_socket)
	{
		// epoll注册新套接字
		register_socket(client_socket);
	}

	// 处理某个监听的套接字的事件
	void handle_event(int event_fd)
	{
		callback_manager.execute_callback(event_fd);
	}

private:
	int epoll_fd;
	static const int MAX_EVENTS = 10;
	CallbackManager callback_manager;

	// epoll注册套接字封装
	void register_socket(int client_socket)
	{
        epoll_event event;
        event.data.fd = client_socket;
        event.events = EPOLLIN;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &event) == -1) 
            throw std::runtime_error("Failed to add socket to epoll");
	}

	// epoll移除套接字封装 
	void remove_socket(int client_socket)
	{
		if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_socket, nullptr) == -1) 
            throw std::runtime_error("Failed to remove socket from epoll");
        close(client_socket);
	}


};



#endif
