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
		std::cout<<"begin to handle request"<<std::endl;
		char buffer[BUFFER_SIZE];
	    ssize_t bytes_read, bytes_written, total_written;
	    
	    bytes_read = readall(fd, buffer, sizeof(buffer));
		std::cout<<"finish reading "<<bytes_read<<" of bytes"<<std::endl;
	    if (bytes_read == -1)
		{
	        perror("read");
	        return;
	    }
	
		bytes_written = writeall(fd, buffer, bytes_read);
		std::cout<<"finish writing "<<bytes_written<<" of bytes"<<std::endl;
		if (bytes_written != bytes_read)
		{
			perror("write");
			return;
		}
	}

private:
	int fd;
	ThreadPool& thread_pool;
};



#endif
