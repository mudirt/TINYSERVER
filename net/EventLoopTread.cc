#include "net/EventLoopThread.h"
#include "net/EventLoop.h"


EventLoopThread::EventLoopThread()
    :loop_(nullptr),
    exiting_(false),
    mutex_(),
    cond_(),
    thread_(std::bind(&EventLoopThread::threadFunc,this))
{

}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    if(loop_ != nullptr)
    {
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop()
{
    //开启新线程
    thread_.start();

    EventLoop *loop = nullptr;
    {

        std::unique_lock<std::mutex> lock(mutex_);
        while(loop_ == nullptr)
        {
            cond_.wait(lock);
        }
        loop = loop_;
    }
    return loop;


}


void EventLoopThread::threadFunc()
{
    EventLoop loop;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;           //等EventLoop创建完成再唤醒
        cond_.notify_one();
    }

    loop.loop();//阻塞此处，为subloop；

    std::unique_lock<std::mutex> lock(mutex_);//函数结束自动丢弃
    loop_ = nullptr;
}