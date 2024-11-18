#include "Connection.h"

Connection::Connection(EventLoop *loop, std::unique_ptr<Socket> clientsock) : loop_(loop), clientsock_(std::move(clientsock)), disconnected_(false), clientchannel_(new Channel(loop_, clientsock_->fd()))
{
    // 将新连接添加到 epoll 监听中
    // ev.data.fd = clientsock->fd();
    // ev.events = EPOLLIN | EPOLLET;
    // epoll_ctl(epollfd, EPOLL_CTL_ADD, clientsock->fd(), &ev);
    // ep.addfd(clientsock->fd(), EPOLLIN | EPOLLET);

    clientchannel_->setreadcallback(bind(&Connection ::onmessage, this));
    clientchannel_->setclosecallback(bind(&Connection ::closecallback, this));
    clientchannel_->seterrorcallback(bind(&Connection ::errorcallback, this));
    clientchannel_->setwritecallback(bind(&Connection ::writecallback, this));

    clientchannel_->useet();
    clientchannel_->enablereading();
}

Connection::~Connection()
{
    // delete clientsock_;
    // delete clientchannel_;
    //cout << "Connection::~Connection()" << endl;
}

int Connection::fd() const
{
    return clientsock_->fd();
}
// 返回ip_成员
std::string Connection ::ip() const

{
    return clientsock_->ip();
}

// 返回port_成员
uint16_t Connection ::port() const
{
    return clientsock_->port();
}

// 关闭连接的回调函数,供Channel调用
void Connection ::closecallback()
{
    disconnected_ = true;
    clientchannel_->remove();
    closecallback_(shared_from_this());
}

// 错误回调函数，供Channel调用
void Connection ::errorcallback()
{
    // 关闭连接,删除Channel
    disconnected_ = true;
    clientchannel_->remove();

    errorcallback_(shared_from_this());
}

// 设置读事件回调函数,供TcpServer调用
void Connection ::setclosecallback(const std::function<void(spConnection)> &cb)
{
    closecallback_ = cb;
}

// 设置错误回调函数,供TcpServer调用
void Connection ::seterrorcallback(const std::function<void(spConnection)> &cb)
{
    errorcallback_ = cb;
}

// 处理客户端数据的回调函数
void Connection ::setonmessagecallback(function<void(spConnection, string &)> fn)
{
    onmessagecallback_ = fn;
}

// 设置发送完成回调函数
void Connection ::setsendcompletecallback(function<void(spConnection)> fn)
{
    sendcompletecallback_ = fn;
}

// 处理客户端发送过来的信息
void Connection ::onmessage()
{
    char buff[1024];
    while (true)
    {
        bzero(&buff, sizeof(buff));
        ssize_t nread = read(fd(), buff, sizeof(buff));
        if (nread > 0)
        {
            // nread表示本次读到的字节数，>0表示读到数据
            //  cout << "recv(eventfd=" << fd() << "):" << buff << endl;
            //  send(fd(), buff, strlen(buff), 0);
            inputbuffer_.append(buff, nread); // 将读到的内容追加到输入缓冲区
        }
        else if (nread == -1 && errno == EINTR)
        { // 信号中断，继续读
            continue;
        }
        else if (nread == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            std::string message;
            // 所有的输入数据都读完了
            while (1)
            {
               if(inputbuffer_.pickup_msg(message) == false) break;
                
                lastatime_ = Timestamp::now(); // 更新最后一次收到数据的时间
               // cout<<"lastatime_:"<<lastatime_.toString()<<endl;

                // 调用onmessagecallback_处理收到的消息
                onmessagecallback_(shared_from_this(), message); // 处理收到的消息
            }

            break;
        }
        else if (nread == 0)
        {
            // 客户端关闭连接
            closecallback();
            break;
        }
    }
}

// 发送数据
void Connection::send(const char *data, size_t len)
{

    if (disconnected_ == true)
    {
        cout << "client is disconnected, can not send data!" << endl;
        return;
    }
    // 不能直接调用sendinloop，因为可能在其他线程中调用，需要转到loop线程中调用
    if (loop_->isInLoopThread())
    {
        //cout << "sendinloop in loop thread:" << syscall(SYS_gettid) << endl;
        sendinloop(data, len);
    }
    else
    {
        //cout << "sendinloop in other thread:" << syscall(SYS_gettid) << endl;
        // 不能直接调用sendinloop，因为可能在其他线程中调用，需要转到loop线程中调用
        loop_->queueInLoop(std::bind(&Connection::sendinloop, this, data, len));
    }
    outputbuffer_.append_head(data, len); // 将要发送的数据追加到输出缓冲区

    clientchannel_->enablewriting(); // 注册写事件
}

void Connection::sendinloop(const char *data, size_t len)
{

    outputbuffer_.append_head(data, len); // 将要发送的数据追加到输出缓冲区

    clientchannel_->enablewriting(); // 注册写事件
}

//  写事件回调回调函数，供Channel调用
void Connection::writecallback()
{
    int writelen = ::send(fd(), outputbuffer_.data(), outputbuffer_.size(), 0);
    if (writelen > 0)
        outputbuffer_.erase(0, writelen); // 发送成功，删掉发送成功的部分

    if (outputbuffer_.size() == 0)
    {
        clientchannel_->disablewriting();
        sendcompletecallback_(shared_from_this());
    }
}

bool Connection::timeout(time_t now, int val)
{
    return (now - lastatime_.toInt() > val);
}