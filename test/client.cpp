#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

void set_nonblocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl(F_GETFL)");
        exit(EXIT_FAILURE);
    }
    flags |= O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, flags) == -1) {
        perror("fcntl(F_SETFL)");
        exit(EXIT_FAILURE);
    }
}

void communicate_with_server(const std::string& server_ip, int server_port) {
    // 创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 准备服务器的地址信息
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // 连接到服务器
    if (connect(sockfd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
        perror("connect");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Connected to server\n";

    // 设置非阻塞模式
    set_nonblocking(sockfd);

    // 循环发送和接收消息
    while (true) {

		while(true)
		{
			// 发送消息
			std::string message;
			std::cout << "Enter message: ";
			std::getline(std::cin, message);
	
	        if (message.empty()) {
	            std::cout << "finish this message\n";
	            break;
	        }
	
	        if (send(sockfd, message.c_str(), message.size(), 0) == -1) {
	            perror("send");
	            close(sockfd);
	            exit(EXIT_FAILURE);
	        }
		}


		std::cout<<"recving..."<<std::endl;
        // 接收服务器响应
        char buffer[1024];
        ssize_t bytes_received;
        while ((bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[bytes_received] = '\0';
            std::cout << "Server: " << buffer << std::endl;
        }

        if (bytes_received == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("recv");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
    }

    // 关闭套接字
    close(sockfd);
}

int main() {
    std::string server_ip = "127.0.0.1"; // 服务器IP地址
    int server_port = 33333; // 服务器端口号

    communicate_with_server(server_ip, server_port);

    return 0;
}

