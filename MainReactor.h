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
#include "SubReactor.h"


class MainReactor
{
public:
    MainReactor() {}
    ~MainReactor()
	{
		for(auto sub_reactor : sub_reactors)
			delete sub_reactor;
	}
    void start()
    {
        // connect to port 33333 and listen to the port
        connect();

        // 接收来自客户端的连接并将连接传递给subreactor
        while(true)
        {
			int client_socket accept_client();
           
            if (client_socket < 0)
			{
                std::cerr << "Failed to accept connection" << std::endl;
                continue;
            }
            
            std::cout << "Connection accepted from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;
            
            // 这里可以将client_socket传递给subreactor进行处理
            handle_client(client_socket);
        }
    }

private:
    int server_socket;
	std::vector<SubReactor*> sub_reactors;
    void connect()
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

	int accept_client()
	{
		struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
		return client_socket;
	}

	void handle_client(int client_socket)
	{
		std::cout<<"handling client..."<<std::endl;
		if(sub_reactors.size() < 3)
		{
			sub_reactors.push_back(new SubReactor());
			sub_reactors.back()->start();
		}
		int sub_id = choose_sub();
		sub_reactors[sub_id]->handle_client(client_socket);
	}

	int choose_sub()
	{
		return rand()%(int)sub_reactors.size();
	}

};

#endif
