#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>

#define PORT 12345
#define MAX_CLIENTS 30
#define BUFFER_SIZE 1024

void startup(int &server_fd, struct sockaddr_in &address, int &addrlen)
{
	// 创建监听套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 绑定套接字
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 监听传入连接
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d \n", PORT);

	addrlen = sizeof(address);
}

int main() {
	int server_fd, addrlen;
	struct sockaddr_in address;
	startup(server_fd, address, addrlen);

	int client_socket[MAX_CLIENTS];
    // 初始化客户端socket数组
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_socket[i] = 0;
    }

    int max_sd;
    char buffer[BUFFER_SIZE];
    fd_set readfds;

	while (1) {
        // 清空并设置文件描述符集
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;// max_sd指当前要监视的文件集合中的最大文件描述符

        // 添加活动的客户端套接字到集合中
        for (int i = 0; i < MAX_CLIENTS; i++) 
		{
            int sd = client_socket[i];
            if (sd > 0)
                FD_SET(sd, &readfds);
            if (sd > max_sd)//用新加入的客户端套接字更新max_sd
                max_sd = sd;
        }

        // 等待活动
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

		int new_client;
        // 检查是否有新的传入连接
		// FD_ISSET: File Descriptor IS SET, 用于检查在 fd_set 数据结构中的指定文件描述符是否被设置
        if (FD_ISSET(server_fd, &readfds)) {
			//通过 FD_ISSET 宏，可以检查每个文件描述符是否在 select 返回的文件描述符集合中
			//如果server_fd在可读集合(readfds)中则表示有新连接
            if ((new_client = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("New connection, socket fd is %d, ip is: %s, port: %d\n",
                   new_client, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // 添加新套接字到客户端数组
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_client;
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }
        }

        // 处理其他客户端的I/O操作
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = client_socket[i];
			int valread;
            if (FD_ISSET(sd, &readfds)) {
				// 如果 read 函数返回值为 0，表示客户端已经断开连接。
                if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0) {
                    // 客户端断开连接
                    getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                    printf("Host disconnected, ip %s, port %d \n",
                           inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    close(sd);
                    client_socket[i] = 0;
                } else {
                    buffer[valread] = '\0';
                    printf("Received message: %s\n", buffer);
                    send(sd, buffer, valread, 0);
                }
            }
        }
    }

    return 0;
}
