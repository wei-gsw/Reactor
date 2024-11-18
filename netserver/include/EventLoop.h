#pragma once
#include "Epoll.h"
#include "Channel.h"
#include <memory>
#include <sys/syscall.h>
#include <unistd.h>
#include <queue>
#include <mutex>
#include <sys/eventfd.h>
#include <map>
#include "Connection.h"
#include <mutex>
#include <atomic>
#include <sys/timerfd.h>
class Epoll;
class Channel;
class Connection;
using spConnection = std::shared_ptr<Connection>;
// 事件循环类
class EventLoop
{
private:
    int timeval_;           // 定时器设置的时间
    int timeout_;           // 定时器超时时间
    std::mutex mutex_;      // 事件循环锁conn
    std::atomic_bool stop_; // 事件循环停止标志
    // 每个循环只有一个epoll
    std::unique_ptr<Epoll> ep_;
    function<void(EventLoop *)> setepoll_timeout_cb_;

    pid_t threadid_;
    // 事件循环线程被eventfd唤醒后的执行队列
    std::queue<std::function<void()>> queue_;
    // 事件队列锁
    std::mutex connection_mutex_;
    // 唤醒事件fd
    int wakeupfd_;
    // 唤醒事件fd对应的Channel
    std::unique_ptr<Channel> wakeupChannel_;
    // 定时器fd
    int timerfd_;
    // 定时器fd对应的Channel
    std::unique_ptr<Channel> timerChannel_;

    // 是否是主事件循环
    bool ismainloop_;

    //
    //
    map<int, spConnection> connections_;

    function<void(int)> timecallback_; // 定时器回调函数

public:
    // 构造函数创建epoll对象
    EventLoop(bool ismainloop, int timeval = 30, int timeout = 80);
    // 销毁ep_
    ~EventLoop();
    // 运行事件循环
    void run();
    // 停止事件循环
    void stop();

    // 判断当前线程是否是EventLoop所在线程
    bool isInLoopThread() const;

    // 设置epoll超时回调函数
    void set_epoll_timeout_cb(function<void(EventLoop *)> cb);
    // 将channel和需要监视的事件挂到红黑树上
    void updateChannel(Channel *ch);
    // 从红黑树上移除channel
    void removeChannel(Channel *ch);

    // 向事件循环线程的执行队列中添加任务
    void queueInLoop(function<void()> cb);
    // 唤醒事件循环线程
    void wakeup();
    //  处理唤醒事件
    void handlewakeup();
    // 处理定时器事件
    void handletimer();

    // zh: 增加连接管理
    void addConnection(spConnection conn);

    // zh:设置定时器回调函数
    void setTimeCallback(function<void(int)> cb);
};