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
		char buffer[BUFFER_SIZE];
	    ssize_t bytes_read, bytes_written, total_written;
	   
		bytes_read = read(fd, buffer, sizeof(buffer));
		std::cout<<"finish reading"<<std::endl;
		bytes_written = write(fd, buffer, bytes_read);
		std::cout<<"finish writing"<<std::endl;
	}

private:
	int fd;
	ThreadPool& thread_pool;
};



#endif
