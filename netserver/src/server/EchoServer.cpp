#include "EchoServer.h"

EchoServer::EchoServer(const string &ip, const uint16_t port, int threadnum, int worksnum) : tcpserver_(ip, port, threadnum), threadpool_(worksnum, "WORKS")
{
    tcpserver_.setnewconnectioncb(std::bind(&EchoServer::HannelNewConnection, this, std::placeholders::_1));
    tcpserver_.setcloseconnectioncb(std::bind(&EchoServer::HannelClose, this, std::placeholders::_1));
    tcpserver_.seterrorconnectioncb(std::bind(&EchoServer::HannelError, this, std::placeholders::_1));
    tcpserver_.setonmessagecb(std::bind(&EchoServer::HannelMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpserver_.setsendcompetecb(std::bind(&EchoServer::HannelSendCompete, this, std::placeholders::_1));
    tcpserver_.setepolltimeoutcb(std::bind(&EchoServer::HannelEpollTimeout, this, std::placeholders::_1));
}
EchoServer ::~EchoServer()
{
}

void EchoServer::start()
{
    tcpserver_.start();
}
// 停止服务
void EchoServer::stop(){
    //停止工作线程
    threadpool_.stop();
    cout << "Stop threadpool!" << endl;
    //停止IO线程
    tcpserver_.stop();
}

// // 处理客户端连接请求
void EchoServer::HannelNewConnection(spConnection conn)
{
    cout << "New connection from(fd=" << conn->fd() << "):" << conn->ip() << ":" << conn->port() << endl;
    //cout << "New connection in!" << endl;
    // cout << "EchoServer::HannelNewConnection() thread is " << syscall(SYS_gettid) << endl;
}
// 关闭客户端的连接，在Connection类中回调此函数,并从connections_中删除
void EchoServer::HannelClose(spConnection conn)
{
     cout << "Connection disconnected.(fd=" << conn->fd() << "):" << conn->ip() << ":" << conn->port() << endl;
    // cout << "Connection closed!" << endl;
}
// 客户端连接出错，在Connection类中回调此函数,并从connections_中删除
void EchoServer::HannelError(spConnection conn)
{
   // cout << "Connection error!" << endl;
}
// 处理客户端发送的消息，在Connection类中回调此函数
void EchoServer::HannelMessage(spConnection conn, string &msg)
{
    // cout << "EchoServer::HannelMessage() thread is " << syscall(SYS_gettid) << endl;
    if (threadpool_.size() == 0)
    {
        OnMessage(conn, msg);
    }
    else
    {
        threadpool_.addTask(std::bind(&EchoServer::OnMessage, this, conn, msg));
    }
}
void EchoServer::OnMessage(spConnection conn, string &msg)
{
    // cout << "recv(eventfd=" << fd() << "):" << message << endl;
    msg = "reply:" + msg;
    conn->send(msg.data(), msg.size());
}
// 发送完数据后的处理函数
void EchoServer::HannelSendCompete(spConnection conn)
{
    //cout << "Send complete!" << endl;
}
// 处理epoll超时事件
void EchoServer::HannelEpollTimeout(EventLoop *loop)
{
    cout << "Epoll timeout!" << endl;
}
