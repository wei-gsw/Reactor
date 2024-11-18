
#include "TreadPool.h"

TreadPool::TreadPool(size_t num_threads, const std::string &type) : stop_(false), threadtype_(type)
{
    for (size_t i = 0; i < num_threads; i++)
    {
        // 启动treadnum个线程，每个线程阻塞在条件变量上
        treads_.emplace_back([this]
                             {
                             //std::cout << "create "<<threadtype_.c_str()<<"thread:" << syscall(SYS_gettid)<<std::endl;
                            
                                 while (stop_ == false)
                                 {
                                     // 存放出队函数
                                     std::function<void()> task;

                                     {
                                         // 锁的作用域开始
                                         std::unique_lock<std::mutex> lock(this->mutex_);

                                         // 等待生产者的条件变量
                                         this->condition_.wait(lock, [this]
                                                               { return ((this->stop_ == true) || (this->taskQueue_.empty() == false)); });
                                         // 线程池停止之前，如果队列还有任务，执行完再退出
                                         if ((this->stop_ == true) && (this->taskQueue_.empty() == true)) return;  

                                         task = std::move(this->taskQueue_.front());
                                         this->taskQueue_.pop();
                                     } // 锁的作用域结束
                                     //printf("%s(%ld)execute task.\n", threadtype_.c_str(), syscall(SYS_gettid));

                                     task();
                                 } });
    }
}

// 停止线程池
void TreadPool::stop()
{
    if (stop_ == true)
        return;

    stop_ = true;
    condition_.notify_all();
    for (auto &it : treads_)
    {
        it.join();
    }
}
TreadPool ::~TreadPool()
{
    stop();
}

void TreadPool::addTask(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        taskQueue_.push(task);
    }
    condition_.notify_one();
}

// 线程池的大小
size_t TreadPool::size() const
{
    return treads_.size();
}