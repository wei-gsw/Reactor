#include "Socket.h"

int createnonblocking()
{
    int listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
    if (listenfd < 0)
    {
        perror("socket() failed");
        exit(-1);
    }
    return listenfd;
}

Socket::Socket(int fd) : fd_(fd)
{
}

Socket::~Socket()
{
    close(fd_);
}

int Socket::fd() const
{
    return fd_;
}
void Socket::setreuseaddr(bool on)
{
    int opt = on ? 1 : 0;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof opt));
}

void Socket::setrenodelay(bool on)
{
    int opt = on ? 1 : 0;
    setsockopt(fd_, SOL_SOCKET, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof opt));
}

void Socket::setkeepalive(bool on)
{
    int opt = on ? 1 : 0;
    setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &opt, static_cast<socklen_t>(sizeof opt));
}

void Socket::setreuseport(bool on)
{
    int opt = on ? 1 : 0;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof opt));
}

void Socket::bind(const InetAddress &servaddr)
{
    if (::bind(fd_, servaddr.addr(), sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        close(fd_);
        exit(-1);
    }
    setipport(servaddr.ip(), servaddr.port());
}
void Socket::listen(int nn)
{

    if (::listen(fd_, 128) != 0)
    {
        perror("listen failed");
        close(fd_);
        exit(-1);
    }
}

int Socket::accept(InetAddress &clientaddr)
{
    sockaddr_in peeraddr;
    socklen_t len = sizeof(peeraddr);
    int clientfd = accept4(fd_, (sockaddr *)&peeraddr, &len, SOCK_NONBLOCK);
    // 检查 accept4 是否成功
    if (clientfd == -1)
    {
        perror("accept4 failed"); // 输出错误信息
        return -1;                // 返回 -1 表示接收失败
    }
    clientaddr.setaddr(peeraddr);

    return clientfd;
}

// 返回ip_成员
std::string Socket::ip() const
{
    return ip_;
}

// 返回port_成员
uint16_t Socket ::port() const
{
    return port_;
}

void Socket::setipport(const std::string &ip, uint16_t port)
{
    ip_ = ip;
    port_ = port;
}