#include "net/Acceptor.h"
#include "net/EventLoop.h"
#include "net/Channel.h"
#include "net/socket.h"
#include "net/Address.h"

static int CreateNonBlocking()
{
    int sockfd = ::socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC,IPPROTO_TCP);
    if(sockfd<0)
    {
        std::cerr<<"listen socket create erro"<<std::endl;
    }
    return sockfd;
}


Acceptor::Acceptor(EventLoop*loop,const Address& listenaddr,bool reuseport):
    loop_(loop),
    AcceptSocket_(CreateNonBlocking()),
    AcceptorChannel_(loop,AcceptSocket_.fd()),
    listening_(false)
{
    AcceptSocket_.setReuseAddr(reuseport);
    AcceptSocket_.setReusePort(true);
    AcceptSocket_.bind(listenaddr);
    //acceptorchannel注册可读事件
    AcceptorChannel_.setReadCallback(std::bind(&Acceptor::handleRead,this));

}

void Acceptor::listen()
{
    listening_ = true;
    AcceptSocket_.listen();
    AcceptorChannel_.enableReading();
}

void Acceptor::handleRead()
{
    Address peerAddr;
    int connfd = AcceptSocket_.accept(&peerAddr);

    if(connfd>0)
    {
        if (NewConnectionCallBack_)
        {
            NewConnectionCallBack_(connfd,peerAddr);
        }
        else
        {
            ::close(connfd);
        }
    }
    else
    {
        std::cerr<<"accept() erro"<<std::endl;
    }

}