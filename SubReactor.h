#ifndef SUB_REACTOR_H
#define SUB_REACTOR_H

#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include <stdexcept>
#include <map>
#include "ThreadPool.h"
#include "Request.h"
#include "Epoll.h"


class SubReactor
{
public:
	SubReactor(int thread_num=5)
	{
        epoll_fd = epoll_create1(0);
        if (epoll_fd == -1) 
            throw std::runtime_error("Failed to create epoll file descriptor");
        events = new epoll_event[MAX_EVENTS];
    }

	~SubReactor()
	{
		close(epoll_fd);
		delete[] events;
	}

	// 子reactor的主进程
	void start()
	{
		while (true) 
        {
            int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
            if (event_count == -1) 
                throw std::runtime_error("Epoll wait error");
            for (int i=0; i<event_count; ++i) 
                handle_socket((Request*)events[i].data.ptr);
        }
	}

	// 加入新的客户端套接字
	void add_socket(int client_socket)
	{
		Request* request = new Request(client_socket);
        __uint32_t epoll_events = EPOLLIN | EPOLLET | EPOLLONESHOT;
		/*
		EPOLLIN: 表示对应的文件描述符可以读（包括对端套接字关闭）。
		EPOLLET: 表示将 epoll 设置为边缘触发（Edge Triggered）模式。
		EPOLLONESHOT: 表示仅监听一次事件，当监听完此事件后，如果还需要继续监听这个 socket，需要再次加入到 epoll 监听队列中。
		*/
		epoll_add(epoll_fd, client_socket, static_cast<void*>(request), epoll_events);
	}

private:
	int epoll_fd;
	static const int MAX_EVENTS = 10;
	struct epoll_event* events;

	// 处理某个监听的套接字的事件
	void handle_socket(Request* request)
	{
		request->run();
	}

};



#endif
