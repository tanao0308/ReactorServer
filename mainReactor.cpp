#ifndef MAIN_REACTOR
#define MAIN_REACTOR

class MainReactor
{
public:
    MainReactor() {}
    ~MainReactor() {}
    void start()
    {
        // connect to port 33333 and listen to the port
        connect();

        // 接收来自客户端的连接并将连接传递给subreactor
        while(true)
        {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
            
            if (client_socket < 0) {
                std::cerr << "Failed to accept connection" << std::endl;
                continue;
            }
            
            std::cout << "Connection accepted from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;
            
            // 这里可以将client_socket传递给subreactor进行处理
            handleClient(client_socket);
        }
    }

private:
    int server_socket;
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

};

#endif