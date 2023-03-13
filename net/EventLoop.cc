#include "net/EventLoop.h"


__thread EventLoop *t_loopInThisThread = nullptr; //_thread 静态初始化，保证线程内只创建一次对象

const int kPollerTimeMs = 10000;


EventLoop::EventLoop(/* args */):
    looping_(false),
    quit_(false),
    threadId_(CurrentThread::tid()),  
    poller_()
{
    
}

EventLoop::~EventLoop()
{
}

void EventLoop::updateChannel(Channel* channel)
{
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    poller_->removeChannel(channel);
}

void EventLoop::loop()
{
    looping_ = true;
    quit_ = false;

    while(!quit)
    {
        activeChannels_.clear();

        pollReturnTime_ = poller_->Poll(kPollerTimeMs,&activeChannels_);
        for(Channel *channel : activeChannels_)
        {
            channel->handleEvent(pollReturnTime_);
        }

        doPendingFunctors();
    }
    looping_ = false;
}

void EventLoop::quit()
{
    quit_ = true;

    if(isInLoopThread())
    {
        wakeup();
    }
}

void EventLoop::runInLoop(Functor cb)
{
    if(isInLoopThread())
    {
        cb();
    }

    else
    {
        queueInLoop(cb);
    }

}

void EventLoop::queueInLoop(Functor cb)
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pendingFunctors_.emplace_back(cb);
    }
    //唤醒所在线程
    
    if(!isInLoopThread()||callingPendingFunctors_)
    {
        wakeup();
    }
}


void EventLoop::wakeup()
{
    uint64_t one = 1;
    size_t n = write(wakeupFd_,&one,sizeof(one));
    if(n!=sizeof(one))
    {
        std::cerr<<"EventLoop::wakeup writes"<<n<<"bytes insetad of 8"<<std::endl;
    }
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    size_t n = read(wakeupFd_,&one,sizeof(one));
    if(n!=sizeof(one))
    {
        std::cerr<<"EventLoop::wakeup handleRead()reads"<<n<<"bytes insetad of 8"<<std::endl;
    }
}

void EventLoop::updateChannel(Channel *channel)
{
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel)
{
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel)
{
    return poller_->hasChannel(channel);    
}

void EventLoop::doPendingFunctors()
{
    callingPendingFunctors_ = true;
    std::vector<Functor> functors;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for(Functor functor:functors)
    {
        functor();
    }
    callingPendingFunctors_ = false;

}
