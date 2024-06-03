#ifndef HTTP_H
#define HTTP_H

const int BUFFER_SIZE = 1000;

ssize_t readall(int fd, char *buffer, size_t count)
{
    size_t bytes_left = count;
    ssize_t bytes_read;
    char *ptr = buffer;

    while (bytes_left > 0)
	{
        bytes_read = read(fd, ptr, bytes_left);
        if (bytes_read == -1)
		{
            if (errno == EINTR)
                continue;
            perror("read");
            return -1;
        }
		else if (bytes_read == 0)
            break;
        bytes_left -= bytes_read;
        ptr += bytes_read;
    }

    return (count - bytes_left); // 返回实际读取的字节数
}

ssize_t writeall(int fd, char *buffer, size_t count)
{
    size_t bytes_left = count;
    ssize_t bytes_written;
    const char *ptr = buffer;

    while (bytes_left > 0)
	{
        bytes_written = write(fd, ptr, bytes_left);
        if (bytes_written == -1)
		{
            if (errno == EINTR)
                continue;
            perror("write");
            return -1;
        }
        bytes_left -= bytes_written;
        ptr += bytes_written;
    }

    return count; // 返回写入的总字节数
}

void echo_response(int fd)
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

void http_response(int fd)
{

}


#endif
