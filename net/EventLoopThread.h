#ifndef NET_EVENTLOOP_THREAD
#define NET_EVENTLOOP_THREAD


#include <functional>
#include <condition_variable>
#include "base/noncopyable.h"
#include "base/Thread.h"

class EventLoop;
class EventLoopThread : noncopyable
{
public:
    EventLoopThread();
    
    ~EventLoopThread();

    EventLoop *startLoop();

private:
    void threadFunc();

    EventLoop *loop_;
    bool exiting_;
    std:: mutex mutex_;
    std::condition_variable cond_;
    Thread thread_;

};



#endif