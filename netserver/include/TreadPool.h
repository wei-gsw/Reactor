#pragma once
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>
#include <iostream>
#include <unistd.h>
#include<sys/syscall.h>
#include<string>
#include <atomic>

class TreadPool
{
public:
    TreadPool(size_t num_threads,const std::string &type);
    ~TreadPool();
    //停止线程池 
    void stop();
    void addTask(std::function<void()> task);

    // 线程池的大小
    size_t size() const;

private:
    // 线程池里的线程
    std::vector<std::thread> treads_;
    // 任务队列
    std::queue<std::function<void()>> taskQueue_;
    // 任务队列的同步互斥锁

    std::mutex mutex_;
    // 任务队列的同步条件变量
    std ::condition_variable condition_;
    // stop，析构函数里设置为true，全部线程退出
    std::atomic<bool> stop_;
    std::string threadtype_;


};