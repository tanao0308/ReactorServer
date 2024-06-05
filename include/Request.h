#ifndef REQUEST_H
#define REQUEST_H

#include "ThreadPool.h"
#include "Http.h"

class Request
{
public:
	Request(int in_fd): fd(in_fd), thread_pool(ThreadPool::getInstance())
	{
	}
	~Request()
	{
	}
	int getfd() { return fd; }
	void run()
	{
		thread_pool.submit([this]{ this->handle_request(); });
	}
	void handle_request()
	{
		std::cout<<"--------------------"<<std::endl;
        char buffer[1024];

		// 接收客户端信息
        int length;
		if((length = recv_till(fd, buffer, '\n')) == -1)
		{
	        std::cout<<"error in client recv"<<std::endl;
	        close(fd);
	        exit(EXIT_FAILURE);
	    }
		buffer[length] = '\0';
		std::cout<<"recv: "<<buffer<<" length is "<<length<<std::endl;
		std::cout<<"finish recving from fd "<<fd<<std::endl;
	
		// 回复信息
		int bytes_send;
		if((bytes_send = send_all(fd, buffer, length)) != length)
		{
	        std::cout<<"error in client send"<<std::endl;
	        close(fd);
	        exit(EXIT_FAILURE);
	    }
		buffer[bytes_send] = '\0';
		std::cout<<"send: "<<buffer<<" length is "<<length<<std::endl;
		std::cout<<"finish sending to fd "<<fd<<std::endl;
	}

private:
	int fd;
	ThreadPool& thread_pool;
};



#endif
