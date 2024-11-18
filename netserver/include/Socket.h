#pragma once
#include <iostream>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include "InetAddress.h"
#include <unistd.h>
using namespace std;

// 创建非阻塞fd
int createnonblocking();

class Socket
{
private:
    // socket持有的fd，在构造函数里传进来
    const int fd_;
    // 如果是listenfd，存放服务端监听的ip和port，如果是客户端连接clientfd，存放对端的ip和port
    std::string ip_;
    uint16_t port_;

public:
    Socket(int fd);
    ~Socket();

    int fd() const; // 返回fd成员
    // 返回ip_成员
    std::string ip() const;
    // 返回port_成员
    uint16_t port() const;

    // 设置setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof opt));
    void setreuseaddr(bool on);
    // setsockopt(listenfd, SOL_SOCKET, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof opt));
    void setrenodelay(bool on);
    // setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, &opt, static_cast<socklen_t>(sizeof opt));
    void setkeepalive(bool on);
    // setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof opt));
    void setreuseport(bool on);

    void bind(const InetAddress &servaddr);
    void listen(int nn = 128);
    int accept(InetAddress &clientaddr);

    // 设置ip和port成员
    void setipport(const std::string &ip, uint16_t port);
};
