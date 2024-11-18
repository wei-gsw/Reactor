#include "Acceptor.h"
#include "Channel.h"
#include <functional>

Acceptor::Acceptor(EventLoop *loop, string ip, const uint16_t port) : loop_(loop), servsock_(createnonblocking()), accepetchannel_(loop_, servsock_.fd())
{
    // Socket servsock(createnonblocking());
    // InetAddress servaddr(argv[1], atoi(argv[2]));
    // // 设置listenfd属性
    // servsock.setkeepalive(1);
    // servsock.setrenodelay(1);
    // servsock.setreuseaddr(1);
    // servsock.setreuseport(1);
    // servsock.bind(servaddr);
    // servsock.listen();
    // // Epoll ep;
    // EventLoop loop;
    // Channel *servchannel = new Channel(loop.ep(), servsock.fd()); // 这里函数new了没释放
    // servchannel->setreadcallback(bind(&Channel ::newconnection, servchannel, &servsock));
    // servchannel->enablereading();

    // 创建非阻塞fd并加入到服务段地址中
    // servsock_ = new Socket(createnonblocking());
    InetAddress servaddr(ip, port);
    // 设置listenfd属性
    servsock_.setkeepalive(true);
    servsock_.setrenodelay(true);
    servsock_.setreuseaddr(true);
    servsock_.setreuseport(true);
    servsock_.bind(servaddr);
    servsock_.listen();

    // Epoll ep;
    // EventLoop loop;
    // accepetchannel_ = new Channel(loop_, servsock_->fd()); // 这里函数new了没释放
    accepetchannel_.setreadcallback(std::bind(&Acceptor::newconnection, this));
    accepetchannel_.enablereading();
}

Acceptor::~Acceptor()
{
    // delete servsock_;
    // delete accepetchannel_;
}

// 处理客户端连接请求
void Acceptor ::newconnection()
{
    // 客户端的地址和协议
    InetAddress clientaddr;
    // 注意clientsock只能new出来，不能在栈上，否则析构函数会关闭fd
    // 这里new出来的对象也没有释放,智能指针会自动释放
    std::unique_ptr<Socket> clientsock(new Socket(servsock_.accept(clientaddr)));
    clientsock->setipport(clientaddr.ip(), clientaddr.port());

    newconnectioncallback_(std::move(clientsock));
}

// 设置新连接回调函数
void Acceptor::setnewconnectioncallback(std::function<void(std::unique_ptr<Socket>)> callback)
{
    newconnectioncallback_ = callback;
}