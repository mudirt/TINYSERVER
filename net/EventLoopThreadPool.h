#ifndef EVENTLOOP_THREAD_POOL_H
#define EVENTLOOP_THREAD_POOL_H

#include <vector>
#include <functional>
#include <memory>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{
public:
     using ThreadInitCallback = std::function<void(EventLoop*)>;
    EventLoopThreadPool(EventLoop *baseloop);
    ~EventLoopThreadPool();

    void setThreadNum(int numThreads){numThreads_=numThreads;}
    void start();

    EventLoop *getNextLoop();

private:
    EventLoop *baseloop_;
    bool started_;
    int numThreads_;
    size_t  next_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;

};



#endif

