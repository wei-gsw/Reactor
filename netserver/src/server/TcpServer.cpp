#include "TcpServer.h"
#include <functional>

// 创建acceptor_，并传入loop_，ip，port，
TcpServer::TcpServer(const string &ip, const uint16_t port, int threadnum) : threadnum_(threadnum), mainloop_(new EventLoop(true)), acceptor_(mainloop_.get(), ip, port), threadpool_(threadnum, "IO")
{
    // 主事件循环
    // mainloop_ = new EventLoop;
    mainloop_->set_epoll_timeout_cb(std::bind(&TcpServer ::epolltimeout, this, std::placeholders::_1));

    // acceptor_ = new Acceptor(mainloop_.get(), ip, port);
    acceptor_.setnewconnectioncallback(std::bind(&TcpServer::newconnection, this, std::placeholders::_1));
    // 创建从事件循环
    for (int i = 0; i < threadnum; i++)
    {
        subloops_.emplace_back(new EventLoop(false, 5, 10));
        subloops_[i]->set_epoll_timeout_cb(std::bind(&TcpServer ::epolltimeout, this, std::placeholders::_1));

        subloops_[i]->setTimeCallback(std::bind(&TcpServer ::removeconnection, this, std::placeholders::_1));

        threadpool_.addTask(std::bind(&EventLoop ::run, subloops_[i].get()));
    }
}

// 释放acceptor_
TcpServer::~TcpServer()
{
    // delete acceptor_;
    //  delete mainloop_;
    /* for (auto &it : connections_)
    {
        delete it.second;
    }*/
}

//        loop_.run();
void TcpServer::start()
{
    mainloop_->run();
}

void TcpServer::stop(){
    // 停止主事件循环
    mainloop_->stop();
    cout << "mainloop stop" << endl;
    // 停止从事件循环
    for (int i = 0; i < threadnum_; i++)
    {
        subloops_[i]->stop();
        cout << "subloop stop" << endl;
    }
    //关闭io线程池
    threadpool_.stop();
    cout << "IOthreadpool stop" << endl;
}

// 注册连接到newconnection函数
void TcpServer::newconnection(std::unique_ptr<Socket> clientsock)
{
    // 这里new出来的对象没有释放
    // spConnection conn = new Connection(mainloop_, clientsock);
    spConnection conn(new Connection(subloops_[clientsock->fd() % threadnum_].get(), std::move(clientsock)));
    conn->setclosecallback(std::bind(&TcpServer::closeconnection, this, std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection, this, std::placeholders::_1));
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage, this, std::placeholders::_1, std::placeholders::_2));
    conn->setsendcompletecallback(std::bind(&TcpServer::sendcompete, this, std::placeholders::_1));
   

    {
        std::lock_guard<std::mutex> lock(connections_mutex_);
        // 将连接对象conn的fd和conn对象   存放到connections_map表中tcpserver类中
        connections_[conn->fd()] = conn;
    }

    // 将conn对象添加到对应线程的事件循环中，loop里
    subloops_[conn->fd() % threadnum_]->addConnection(conn);

    // 回调onconnection_函数
    if (newconnectioncb_)
        newconnectioncb_(conn);
}

// 关闭客户端的连接，在Connection类中回调此函数,并将连接从connections_map表中删除
void TcpServer::closeconnection(spConnection conn)
{
    // 回调closeconnection_函数
    closeconnectioncb_(conn);

    // cout << "client(fd=" << conn->fd() << ") disconnected." << endl;
    //  从connections_map表中删除连接
    {
        std::lock_guard<std::mutex> gd(connections_mutex_);
        connections_.erase(conn->fd());
    }

    // 释放连接对象
    // delete conn;
}

// 客户端连接出错，在Connection类中回调此函数
void TcpServer::errorconnection(spConnection conn)
{
    // 回调errorconnection_函数
    errorconnectioncb_(conn);

    // 其他事件，视为错误断开连接
    // cout << "client(fd=" << conn->fd() << ") error!" << endl;

    close(conn->fd());
    //    从connections_map表中删除连接
    {
        std::lock_guard<std::mutex> gd(connections_mutex_);
        connections_.erase(conn->fd());
    }

    // 释放连接对象
    // delete conn;
}

// 处理客户端发来的消息，在Connection类中回调此函数
void TcpServer::onmessage(spConnection conn, string &msg)
{
    // 回调onmessage_函数
    onmessagecb_(conn, msg);
}

// 发送完数据后的处理函数
void TcpServer::sendcompete(spConnection conn)
{
    // cout << "send compete" << endl;

    // 回调sendcompete_函数
    sendcompetecb_(conn);
}

// 处理epoll超时事件
void TcpServer::epolltimeout(EventLoop *loop)
{
    // 处理超时事件
    // cout << "epoll timeout" << endl;
    // 回调epolltimeout_函数
    epolltimeoutcb_(loop);
}

void TcpServer::setnewconnectioncb(function<void(spConnection)> newconnection)
{
    newconnectioncb_ = newconnection;
}
void TcpServer::setcloseconnectioncb(function<void(spConnection)> closeconnection)
{
    closeconnectioncb_ = closeconnection;
}
void TcpServer::seterrorconnectioncb(function<void(spConnection)> errorconnection)
{
    errorconnectioncb_ = errorconnection;
}
void TcpServer::setonmessagecb(function<void(spConnection, string &)> onmessage)
{
    onmessagecb_ = onmessage;
}
void TcpServer::setsendcompetecb(function<void(spConnection)> sendcompete)
{
    sendcompetecb_ = sendcompete;
}
void TcpServer::setepolltimeoutcb(function<void(EventLoop *)> epolltimeout)
{
    epolltimeoutcb_ = epolltimeout;
}

void TcpServer::removeconnection(int fd)
{
    std::lock_guard<std::mutex> gd(connections_mutex_);
    connections_.erase(fd);
}
