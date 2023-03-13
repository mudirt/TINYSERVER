#ifndef NET_CHANNEL
#define NET_CHANNEL

#include <base/types.h>
#include<base/noncopyable.h>
#include <base/Timestamp.h>
#include <functional>
#include <memory>
#include <sys/epoll.h>


class EventLoop;

class Channel :noncopyable
{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop* loop,int fd):   
        loop_(loop),
        fd_(fd),
        events_(0),
        revents_(0),
        index_(-1),
        tied_(false)
    {
    }
    
    ~Channel();


    void handleEvent(Timestamp receiveTime);
    void tie(const std::shared_ptr<void>&);
    
    int fd() const {return fd_;}
    int events() const {return events_;}
    void set_revents(int revt){revents_ = revt;} // 设置Poller返回的发生事件

    // 设置回调函数对象
    // 使用右值引用，延长了cb对象的生命周期，避免了拷贝操作
    void setReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }
    void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
    void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
    void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

    // 设置fd相应的事件状态，update()其本质调用epoll_ctl
    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ &= kNoneEvent; update(); }

    // 返回fd当前的事件状态
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }   

    /**
     * for Poller
     * const int kNew = -1;     // fd还未被poller监视 
     * const int kAdded = 1;    // fd正被poller监视中
     * const int kDeleted = 2;  // fd被移除poller
     */ 
    int index() { return index_; }
    void set_index(int idx) { index_ = idx; }

    // one lool per thread
    EventLoop* ownerLoop() { return loop_; }
    void remove();

private:
    void update();
    void handleEventWithGuard(Timestamp receiveTime);

    /**
     * const int Channel::kNoneEvent = 0;
     * const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
     * const int Channel::kWriteEvent = EPOLLOUT;
     */
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_;
   // 当前Channel属于的EventLoop
    const int fd_;      // fd, Poller监听对象
    int events_;        // 注册fd感兴趣的事件
    int revents_;       // poller返回的具体发生的事件
    int index_;         // 在Poller上注册的情况

    std::weak_ptr<void> tie_;   // 弱指针指向TcpConnection(必要时升级为shared_ptr多一份引用计数，避免用户误删)
    bool tied_;  // 标志此 Channel 是否被调用过 Channel::tie 方法   
    
    // 因为 channel 通道里面能够获知fd最终发生的具体的事件revents
    // 保存事件到来时的回调函数
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;

};



#endif