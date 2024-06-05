#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/Http.h"

void communicate_with_server(const std::string& server_ip, int server_port) {
    // 创建套接字
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 准备服务器的地址信息
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 连接到服务器
    if (connect(server_fd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
        perror("connect");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Connected to server\n";

    // 循环发送和接收消息
    while (true)
	{
		std::cout<<"---------------------"<<std::endl;
		// 发送一行消息
		std::string msg;
		std::cout<<">>>";
		std::getline(std::cin, msg);
		msg.push_back('\n'); // getline会删除结尾的换行符
//		std::cout<<"the message is "<<msg<<"length is "<<msg.size()<<std::endl;

		if(send_all(server_fd, msg.c_str(), msg.size()) != msg.size())
		{
	        std::cout<<"error in client send"<<std::endl;
	        close(server_fd);
	        exit(EXIT_FAILURE);
	    }

        // 接收服务器响应
        char buffer[1024];
		int bytes_recv;
		if((bytes_recv = recv_till(server_fd, buffer, '\n')) == -1)
		{
	        std::cout<<"error in client recv"<<std::endl;
	        close(server_fd);
	        exit(EXIT_FAILURE);
	    }
		buffer[bytes_recv] = '\0';
		std::cout<<buffer<<std::endl;
    }

    // 关闭套接字
    close(server_fd);
}

int main() {
    std::string server_ip = "127.0.0.1"; // 服务器IP地址
    int server_port = 33333; // 服务器端口号

    communicate_with_server(server_ip, server_port);

    return 0;
}

