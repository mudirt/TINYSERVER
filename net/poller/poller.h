#ifndef NET_POLLER
#define NET_POLLER

#include <unordered_map>
#include<vector>
#include<EventLoop.h>

#include "Channel.h"
#include "base/Timestamp.h"
#include "base/noncopyable.h"

class Channel;


class Poller:noncopyable
{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop* loop);

    virtual ~Poller() = default;

    virtual Timestamp Poll(int timeoutMs,ChannelList *activechannels) = 0;
    virtual void updateChannel(Channel *channel) = 0;
    virtual void removeChannel(Channel *channel) = 0;

    bool hasChannel(Channel *channel) const;

    static Poller* newDefaultPoller(EventLoop* loop);


protected:
    using ChannelMap = std::unordered_map<int,Channel*>;
    ChannelMap channels_;

private:
    EventLoop * ownerloop_;

};


#endif