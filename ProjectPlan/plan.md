## 项目架构图
![架构图](./架构图.png)


## 项目需要的类

- main reactor

    功能：监听客户端连接请求，分派客户端套接字给sub reactor

- sub reactor

    功能：监听被分配的套接字，调用线程池处理事件

- thread pool

    功能：调用线程处理事件

- work thread

    功能：处理具体的事件

