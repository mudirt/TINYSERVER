#ifndef EPOLLPOLLER_H
#define EPOLLPOLLER_H

#include "poller.h"
#include "Timestamp.h"
#include<vector>
#include<sys/epoll.h>
#include <unistd.h>

class EPollPoller : public Poller
{
    using EventList = std::vector<epoll_event>;
public:

    EPollPoller(EventLoop* loop);
    ~EPollPoller() override;

    Timestamp Poll(int timeoutMs, ChannelList *activeChannels) override;
    void updateChannel(Channel *channel)override;
    void removeChannel(Channel *channel)override;

private:
    //默认监听事件数量
    static const int kInitEventListSize = 16;

    void fillActiveChannels(int numEvents,ChannelList *activechannels)const;

    void update(int operation,Channel* channel);

    int epollfd_;
    EventList events_;

};

#endif