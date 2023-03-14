#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "base/noncopyable.h"
#include "net/socket.h"
#include "net/Channel.h"

class EventLoop;
class Address;

class Acceptor
{
public:
    using NewConnectionCallBack = std::function<void(int sockfd,const Address&)>;
    void setNewConnectionCallBack (const NewConnectionCallBack cb)
    {
        NewConnectionCallBack_=cb;
    }

    Acceptor(EventLoop*loop,const Address&listenaddr,bool reuseport);
    ~Acceptor();

    bool listening() const {return listening_;}
    void listen();



private:
    void handleRead();
    NewConnectionCallBack NewConnectionCallBack_;
    bool listening_;    
    Socket AcceptSocket_;
    Channel AcceptorChannel_;
    EventLoop* loop_;   //BaseLoop;

};





#endif