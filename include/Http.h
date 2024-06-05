#ifndef HTTP_H
#define HTTP_H

int const BUFFER_SIZE = 1000;

// 从套接字读取指定长度
// 一般用在收到http报文长度后，客户端进行阻塞读取
// const 默认作用于其左边的东西，如果左边无东西则作用于右边，因此尽量保证const左边有东西
int recv_all(int const fd, char *const buffer, const size_t length)
{
	size_t total_recv = 0;
	ssize_t bytes_recv;
	while(total_recv < length)
	{
		/*
		ssize_t recv(int sockfd, void *buf, size_t len, int flags);
		sockfd: 套接字文件描述符。
		buf: 指向存储接收数据的缓冲区。
		len: 缓冲区的长度，即要接收的最大字节数。
		flags: 接收标志，用于修改接收操作的行为。
		flags = 0 表示默认的阻塞接收操作
		*/
		bytes_recv = recv(fd, buffer + total_recv, length - total_recv, 0);
		if(bytes_recv < 0)
		{
			if(errno == EINTR) // 被信号中断
				continue;
			else
				return -1;
		}
		else if(bytes_recv == 0) // 连接被关闭
			break;
		total_recv += bytes_recv;
	}
	return total_recv;
}

// 发送指定长度的数据给套接字
int send_all(int const fd, char const *const buffer, const size_t length)
{
	size_t total_send = 0;
	ssize_t bytes_send;
	while(total_send < length)
	{
		bytes_send = send(fd, buffer + total_send, length - total_send, 0);
		if(bytes_send < 0)
		{
			if(errno == EINTR) // 被信号中断
				continue;
			else
				return -1;
		}
		else if(bytes_send == 0) // 连接被关闭
			break;
		total_send += bytes_send;
	}
//	std::cout<<"send_all!!! ";
//	for(int i=0;i<length;++i) std::cout<<buffer[i];
//	std::cout<<std::endl;
	return total_send;
}

int recv_till(int const fd, char *const buffer, char const end='\n')
{
	size_t total_recv = 0;
	ssize_t bytes_recv;
	while(true)
	{
		bytes_recv = recv(fd, buffer + total_recv, 1, 0);
		buffer[total_recv+1] = '\0';
//		std::cout<<"in recv_till: "<<bytes_recv<<", total_recv="<<total_recv<<", buffer="<<buffer<<std::endl;
		if(bytes_recv < 0)
		{
			if(errno == EINTR) // 被信号中断
				continue;
			else
				return -1;
		}
		
		if(bytes_recv == 0) // 连接被关闭
			break;
		total_recv++;
		if(buffer[total_recv-1] == end) // 读到指定字符（保留此字符）
			break;
	}
//	std::cout<<"in recv_till: total_recv="<<total_recv<<std::endl;
	return total_recv;
}

void echo_response(int fd)
{
	char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written, total_written;
   
	bytes_read = read(fd, buffer, sizeof(buffer));
	std::cout<<"finish reading"<<std::endl;
	bytes_written = write(fd, buffer, bytes_read);
	std::cout<<"finish writing"<<std::endl;


/*
    bytes_read = readall(fd, buffer, sizeof(buffer));
    if (bytes_read == -1)
	{
        perror("read");
        return;
    }

	bytes_written = writeall(fd, buffer, bytes_read);
	if (bytes_written != bytes_read)
	{
		perror("write");
		return;
	}
*/
}

void http_response(int fd)
{

}


#endif
