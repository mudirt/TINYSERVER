    #include "Epollpoller.h"
    #include "types.h"
    const int kNew = -1;    // 某个channel还没添加至Poller          // channel的成员index_初始化为-1
    const int kAdded = 1;   // 某个channel已经添加至Poller
    const int kDeleted = 2; // 某个channel已经从Poller删除

    EPollPoller::EPollPoller(EventLoop* loop) :
        Poller(loop),
        epollfd_(epoll_create1(EPOLL_CLOEXEC)),
        events_(kInitEventListSize)
    {
    }

    EPollPoller::~EPollPoller() 
    {
        ::close(epollfd_);
    }

    Timestamp EPollPoller::Poll(int timeoutMs, ChannelList *activeChannels)
    {
        size_t numEvents = ::epoll_wait(epollfd_,&(*events_.begin()),static_cast<int>(events_.size()),timeoutMs);
        int saveErrno = errno;
        Timestamp now(Timestamp::now());

        if(numEvents > 0)
        {

            fillActiveChannels(numEvents,activeChannels);

            //扩容
            if(numEvents == events_.size())
            {
                events_.resize(events_.size()*2);
            }
        }

        else
        {
            if(saveErrno != EINTR)
            {
                errno = saveErrno;
                std::cerr<<"EPollpoller poll() failed"<<std::endl;
            }
        }
        return now;
                
    }

    void EPollPoller::fillActiveChannels(int numEvents,ChannelList *activeChannels)const
    {
        for (int i =0 ;i< numEvents;++i)
        {
            Channel * channel = static_cast<Channel*>(events_[i].data.ptr);
            channel->set_revents(events_[i].events);
            activeChannels->push_back(channel);
        }
    }


    void EPollPoller::update(int operation,Channel* channel)
    {
        epoll_event event;

        ::bzero(&event,sizeof(event));

        int fd = channel->fd();
        event.events = channel->events();
        event.data.fd = fd;
        event.data.ptr = channel;

        ::epoll_ctl(epollfd_,operation,fd,&event);
    }


    void EPollPoller::updateChannel(Channel *channel)
    {
        const int index = channel->index();

        if(index == kNew || index == kDeleted)
        {
            int fd = channel->fd();
            if(index == kNew)
            {
                channels_[fd] = channel;
            }
            else//index = kDeleted
            {
                assert(channels_.find(fd)!=channels_.end());      //？？？
                assert(channel == channels_[fd]);
            }
            channel->set_index(kAdded);
            update(EPOLL_CTL_ADD,channel);
        }

    }

    void EPollPoller::removeChannel(Channel *channel)
    {
        int fd = channel->fd();
        channels_.erase(fd);

        int index = channel->index();
        if(index = kAdded)
        {
            update(EPOLL_CTL_DEL,channel);
        }
        channel->set_index(kNew);
    }
