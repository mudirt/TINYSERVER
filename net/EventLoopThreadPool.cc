#include "net/EventLoopThreadPool.h"
#include "net/EventLoopThread.h"
#include "net/EventLoop.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseloop):
    baseloop_(baseloop),
    started_ (false),
    numThreads_(0),
    next_(0)
{

}

EventLoopThreadPool::~EventLoopThreadPool()
{

}

void EventLoopThreadPool::start()
{
    started_ = true;
    //循环创建线程
    for(int i=0;i<numThreads_;i++)
    {
        EventLoopThread *t = new EventLoopThread();
        threads_.push_back(std::unique_ptr<EventLoopThread>(t));
        loops_.push_back(t->startLoop());
    }   
}

EventLoop *EventLoopThreadPool::getNextLoop()
{
    EventLoop *loop = baseloop_;

    if(!loops_.empty())
    {
        loop = loops_[next_];
        ++next_;

        if(next_>=loops_.size())
        {
            next_ = 0;
        }
    }
    return loop;

}
