#ifndef NET_EVENTLOOP
#define NET_EVENTLOOP

#include "base/noncopyable.h"
#include "base/Timestamp.h"
#include "base/CurrentThread.h"
#include <functional>
#include <vector>
#include <memory>
#include <atomic>
#include <mutex>
#include "net/poller/poller.h"

class Channel;
class Poller;

class EventLoop :noncopyable
{
public:
    using Functor = std::function<void()>;
    EventLoop(/* args */);
    ~EventLoop();

    void loop();
    void quit();

    void runInLoop(Functor cb);
    void queueInLoop(Functor cb);

    void wakeup();

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);


    bool isInLoopThread() const {return threadId_==CurrentThread::tid();}



private:
    void handleRead();
    void doPendingFunctors();

    using ChannelList = std::vector<Channel*>;
    std:: atomic_bool looping_;
    std:: atomic_bool quit_;
    std::atomic_bool callingPendingFunctors_;  
    const pid_t threadId_;
    Timestamp pollReturnTime_;
    std::unique_ptr<Poller> poller_;
    
    int wakeupFd_; 
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;            // 活跃的Channel
    Channel* currentActiveChannel_;         // 当前处理的活跃channel
    std::mutex mutex_;                      // 用于保护pendingFunctors_线程安全操作
    std::vector<Functor> pendingFunctors_;  // 存储loop跨线程需要执行的所有回调操作


    /* data */

};





#endif