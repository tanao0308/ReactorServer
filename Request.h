#ifndef REQUEST_H
#define REQUEST_H

#include "ThreadPool.h"
#include "Http.h"

class Request
{
public:
	Request(int in_fd): fd(in_fd)
	{
	}
	~Request()
	{
	}
	int getfd() { return fd; }
	void run()
	{
		thread_pool->submit([this]{ this->handle_request(); });
	}
	void handle_request()
	{
		char buffer[BUFFER_SIZE];
	    ssize_t bytes_read, bytes_written, total_written;
	    
	    bytes_read = readall(fd, buffer, sizeof(buffer));
	    if (bytes_read == -1)
		{
	        perror("read");
	        return;
	    }
	
		bytes_written = writeall(fd, buffer, sizeof bytes_read);
		if (bytes_written != bytes_read)
		{
			perror("write");
			return;
		}
	}

private:
	int fd;
	ThreadPool* thread_pool;
};



#endif
