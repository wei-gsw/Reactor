#pragma once
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

using namespace std;

class InetAddress
{
private:
    sockaddr_in addr_;

public:
    InetAddress(const string &ip, uint16_t port); // 监听fd用这个构造函数
    InetAddress(const sockaddr_in addr);          // 客户端连接fd构造函数
    InetAddress() {};
    ~InetAddress();

    const char *ip() const;       // 返回字符串表示的地址
    uint16_t port() const;        // 返回整数端口
    const sockaddr *addr() const; // 返回addr_成员地址
    void setaddr(sockaddr_in clientaddr);//设置addr_成员的值
};
