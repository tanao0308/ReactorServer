#ifndef SUB_REACTOR_H
#define SUB_REACTOR_H

#include <iostream>

class SubReactor
{
public:
	void epoll_add()
	{
	}

private:
	SubReactor() {}
	~SubReactor() {}
	void start()
	{
		while(true)
		{
			// epoll 等待新事件
			// 处理新事件
		}
	}
	void handle_client(int client_socket)
	{
		// epoll注册新套接字
		epoll_add(client_socket);
	}
	
};



#endif
