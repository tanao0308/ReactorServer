#ifndef MAIN_REACTOR_H
#define MAIN_REACTOR_H

#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include "ThreadPool.h"
#include "SubReactor.h"


class MainReactor
{
public:
    MainReactor(int thread_num=5, int sub_reactor_num=3): thread_pool(ThreadPool::getInstance(thread_num))
	{
		for(int i=0; i<sub_reactor_num; ++i)
		{
			// 直接在构造函数内创建reactor number个线程，并在子线程内开启子reactor的事件循环
			SubReactor* sub_reactor = new SubReactor();
			sub_reactors.push_back(sub_reactor);
			/*
			lambda:
			[capture list] (parameter list) -> return type { body }

			capture list：捕获列表，用于捕获外部变量以供 lambda 函数内部使用。捕获列表可以为空，也可以包含一个或多个外部变量的引用或拷贝。捕获列表使用方括号 [] 包围起来。
			parameter list：参数列表，与普通函数的参数列表类似，用于传递参数给 lambda 函数。参数列表可以为空，也可以包含一个或多个参数，使用小括号 () 包围起来。
			return type：返回类型，指定 lambda 函数的返回类型。可以省略返回类型，编译器会自动推断返回类型。使用箭头 -> 指定返回类型。
			body：函数体，包含 lambda 函数的实际执行代码块。使用大括号 {} 包围起来。
			*/
			thread_pool.submit([this, sub_reactor]() { sub_reactor->start(); });
		}
	}

    ~MainReactor()
	{
		for(auto sub_reactor : sub_reactors)
			delete sub_reactor;
	}

    void start()
    {
        init_main_server();

        // 接收来自客户端的连接并将连接传递给subreactor
        while(true)
        {
			int client_socket = get_client();
			if (client_socket < 0)
			{
				std::cerr << "Failed to accept connection" << std::endl;
				continue;
			}
            handle_client(client_socket);
        }
    }

private:
    int server_socket;
	std::vector<SubReactor*> sub_reactors;
	ThreadPool& thread_pool;
	
	// 将主线程连接到某个端口进行初始化
    void init_main_server()
    {
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket < 0)
        {
            std::cerr << "Failed to create socket" << std::endl;
            exit(EXIT_FAILURE);
        }
        
        struct sockaddr_in server_addr;
        std::memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(33333);
        
        if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        {
            std::cerr << "Failed to bind socket" << std::endl;
            close(server_socket);
            exit(EXIT_FAILURE);
        }
        
        if (listen(server_socket, 10) < 0)
        {
            std::cerr << "Failed to listen on socket" << std::endl;
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        std::cout << "Server is listening on port 33333" << std::endl;
    }

	// 阻塞地accept客户端的连接请求
	int get_client()
	{
		struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        std::cout << "Connection accepted from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;
		return client_socket;
	}

	// 将新连接的客户端套接字交给子reactor处理
	void handle_client(int client_socket)
	{
		std::cout<<"handling client..."<<std::endl;
		int sub_id = choose_sub();
		sub_reactors[sub_id]->add_socket(client_socket);
		std::cout<<"added socket."<<std::endl;
	}

	// 随机选择一个子reactor(todo: 负载均衡)
	int choose_sub()
	{
		return rand()%(int)sub_reactors.size();
	}

};

#endif
