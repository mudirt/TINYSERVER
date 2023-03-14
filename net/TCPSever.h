#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <functional>
#include<memory>
#include<unordered_map>
#include <atomic>
#include <string>

#include "net/EventLoop.h"
#include "net/EventLoopThread.h"
#include "net/Address.h"
#include "net/Acceptor.h"
#include "net/EventLoopThreadPool.h"
#include "net/TCPConnection.h"

//using TCPConnetionPtr = std::shared_ptr<TCPConnection>;
class TCPSever
{
public:
    using TCPConnetionPtr = std::shared_ptr<TCPConnection>;
    enum Option
    {
        kNoReusePort,
        kReusePort,
    };

    TCPSever(EventLoop* loop,const Address& listenaddr,Option option = kNoReusePort);
    ~TCPSever();

    void setThreadNum(int num);

    void start();

    EventLoop* getLoop(){return loop_;}



private:
    void newConnection(int sockfd,const Address& peeraddr);
    void removeConnection(const TCPConnetionPtr &conn);
    void removeConnectionInLoop(const TCPConnetionPtr &conn);


    //using ConnetionMap = std::unordered_map<int,std::shared_ptr<TCPConnection>>;
    using ConnetionMap = std::unordered_map<int,TCPConnetionPtr>;
    EventLoop* loop_; //baseloop；
    const std::string ipPort_;

    std::unique_ptr<Acceptor> acceptor_;
    std::shared_ptr<EventLoopThreadPool> threadpool_;
    std::atomic_int started;

    int nextConnId_;   
    ConnetionMap connections_;
};


#endif