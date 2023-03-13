#include "net/Channel.h"
#include "net/EventLoop.h"

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;


Channel::~Channel()
{
    
}


void Channel::tie(const std::shared_ptr<void> &obj)
{

    tie_ = obj;
    tied_ = true;
}


void Channel::update()
{
    loop_->updateChannel(this);
}

void Channel::remove()
{
    loop_->removeChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime)
{
    if(tied_)
    {
        std::shared_ptr<void> gaurd = tie_.lock();//升级成shared_ptr;引用计数大于等于2，防止用户误删
        if(gaurd)//gaurd为空说明Channel的TCPConnection对象已经不再
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}

void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    if((revents_&EPOLLHUP)&&!(revents_&EPOLLIN))
    {
        //回调
    }

    if(revents_&EPOLLERR)
    {
        std::cerr<<"the fd = "<<this->fd()<<"erro"<<std::endl;
    }

    if(revents_ &(EPOLLIN&EPOLLPRI))
    {

    }

}
