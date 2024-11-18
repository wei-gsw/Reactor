#include "InetAddress.h"

// 监听fd用这个构造函数
InetAddress::InetAddress(const string &ip, uint16_t port)
{
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
}

// 客户端连接fd构造函数
InetAddress::InetAddress(const sockaddr_in addr)
{
    addr_ = addr;
}
InetAddress::~InetAddress() {}

// 返回字符串表示的地址
const char *InetAddress::ip() const
{
    return inet_ntoa(addr_.sin_addr);
}
// 返回整数端口
uint16_t InetAddress::port() const
{
    return ntohs(addr_.sin_port);
}

// 返回addr_成员地址,转换成为sockaddr
const sockaddr *InetAddress::addr() const
{
    return (sockaddr *)&addr_;
}

void InetAddress::setaddr(sockaddr_in clientaddr){
    addr_ = clientaddr;
}
