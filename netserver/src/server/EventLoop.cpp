#include "EventLoop.h"

int creattimerfd(int sec)
{
    int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    struct itimerspec timeout;
    memset(&timeout, 0, sizeof(struct itimerspec));
    timeout.it_value.tv_sec = sec;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(tfd, 0, &timeout, 0);
    return tfd;
}

// 构造函数创建epoll对象
EventLoop::EventLoop(bool ismainloop, int timeval, int timeout) : timeout_(timeout), timeval_(timeval),
                                                                  ismainloop_(ismainloop),
                                                                  ep_(new Epoll),
                                                                  wakeupfd_(eventfd(0, EFD_NONBLOCK)),
                                                                  wakeupChannel_(new Channel(this, wakeupfd_)),
                                                                  timerfd_(creattimerfd(timeval)),
                                                                  timerChannel_(new Channel(this, timerfd_)),
                                                                  stop_(false)
{
    // 设置唤醒事件回调函数
    wakeupChannel_->setreadcallback(std::bind(&EventLoop::handlewakeup, this));
    // 注册唤醒事件
    wakeupChannel_->enablereading();
    // 设置定时器事件回调函数
    timerChannel_->setreadcallback(std::bind(&EventLoop::handletimer, this));
    // 注册定时器事件
    timerChannel_->enablereading();
}

// 销毁ep_
EventLoop ::~EventLoop()
{
    // delete ep_;
}

// 运行事件循环
void EventLoop::run()
{
    threadid_ = syscall(SYS_gettid);
    //  事件循环
    while (stop_ == false)
    {
        // 存放epollwait返回事件
        vector<Channel *> chs = ep_->loop(10*1000);
        // 处理时间事件
        if (chs.empty())
        {
            setepoll_timeout_cb_(this);
        }
        else
        {
            // 处理就绪的文件描述符
            for (auto &ch : chs)
            {
                ch->handleevent();
            }
        }
    }
}
// 停止事件循环
void EventLoop::stop(){
    stop_ = true;
    wakeup();
}

void EventLoop::updateChannel(Channel *ch)
{
    ep_->updateChannel(ch);
}


// 设置epoll超时回调函数
void EventLoop::set_epoll_timeout_cb(function<void(EventLoop *)> cb)
{
    setepoll_timeout_cb_ = cb;
}

// 从红黑树上移除channel
void EventLoop::removeChannel(Channel *ch)
{
    ep_->removeChannel(ch);
}

bool EventLoop::isInLoopThread() const
{
    return threadid_ == syscall(SYS_gettid);
}

// 向事件循环线程的执行队列中添加任务
void EventLoop::queueInLoop(function<void()> cb)
{
    {
        std::lock_guard<std::mutex> gd(mutex_);
        queue_.push(cb);
    }
    // 唤醒事件循环线程
    wakeup();
}

// 唤醒事件循环线程
void EventLoop::wakeup()
{
    uint64_t val = 1;
    write(wakeupfd_, &val, sizeof(val));
}

//  处理唤醒事件
void EventLoop::handlewakeup()
{
    uint64_t val;
    // 读出唤醒事件
    read(wakeupfd_, &val, sizeof(val));

    std::function<void()> fn;
    // 处理任务队列
    std::lock_guard<std::mutex> lock(mutex_);
    while (queue_.size() > 0)
    {
        fn = std::move(queue_.front());
        queue_.pop();
        fn();
    }
}

// 处理定时器事件
void EventLoop::handletimer()
{
    // 重新设置定时器
    struct itimerspec timeout;
    memset(&timeout, 0, sizeof(struct itimerspec));
    timeout.it_value.tv_sec = timeval_;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(timerfd_, 0, &timeout, 0);

    if (ismainloop_)
    {
        // cout << "mainloop timerfd_settime" << endl;
    }
    else
    {
        // cout << "EventLoop::handletimer() threadid_ is " << syscall(SYS_gettid) << " fd ";
        //  处理定时器事件
        time_t now = time(0);
        std::vector<int> to_remove;
        for (auto it : connections_)
        {
            // cout << it.first << " ";
            if (it.second->timeout(now, timeout_))
            {
                // cout << "EventLoop::handletimer() threadid_ is" << syscall(SYS_gettid) << "fd ";
                to_remove.push_back(it.first);
            }
        }

        for (int fd : to_remove)
        {
            std::lock_guard<std::mutex> gd(connection_mutex_);
            connections_.erase(fd);
            timecallback_(fd);
        }
    }
}

void EventLoop::addConnection(spConnection conn)
{
    std::lock_guard<std::mutex> gd(connection_mutex_);
    connections_[conn->fd()] = conn;
}

void EventLoop::setTimeCallback(function<void(int)> cb)
{
    timecallback_ = cb;
}