#include "net/TCPSever.h"

TCPSever::TCPSever(EventLoop* loop,const Address& listenaddr,Option option):
    loop_(loop),
    started(0),
    ipPort_(listenaddr.toIpPort()),
    acceptor_(new Acceptor(loop,listenaddr,option)),
    threadpool_(new EventLoopThreadPool(loop)),
    nextConnId_(1)
{

}

void TCPSever::newConnection(int sockfd,const Address& peeraddr)
{
    EventLoop *ioLoop = threadpool_->getNextLoop();

    ++nextConnId_;

    sockaddr_in local;  
    ::bzero(&local,sizeof(local));
    socklen_t addrlen = sizeof(local);

    if(::getsockname(sockfd,(sockaddr*)&local,&addrlen)<0)
    {
        std::cerr<<"getsockname erro"<<std::endl;
    }

    Address localAddr(local);
    TCPConnetionPtr conn(new TCPConnection(ioLoop,nextConnId_,sockfd,localAddr,peeraddr));

    connections_[nextConnId_] = conn;

    conn->setCloseCallback(
        std::bind(&TCPSever::removeConnection,this,std::placeholders::_1)//TCPConnection调用handleclose()时 调用closeCallback，传入其TCPConnectionPtr的shared_from_this();
    );

    ioLoop->runInLoop(
        std::bind(&TCPConnection::connectEstablished,conn)//修改TCPConnection状态为已连接，channel.tie()防止TCPConnction误删。
    );

}

void TCPSever::removeConnection(const TCPConnetionPtr& conn)
{
    loop_->runInLoop(        //判断是否为当前loop
        std::bind(TCPSever::removeConnectionInLoop,this,conn)
    );
}

void TCPSever::removeConnectionInLoop(const TCPConnetionPtr& conn)
{
    connections_.erase(conn->index_);
    EventLoop *ioLoop = conn->getLoop(); //获取conn对应loop；
    ioLoop->queueInLoop(
        std::bind(&TCPConnection::connectDestroyed,conn)
    );

}
