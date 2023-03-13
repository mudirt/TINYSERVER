#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H
#include "base/noncopyable.h"
#include "base/types.h"
#include "net/Buffer.h"
#include "net/Address.h"
#include <atomic>
#include "net/EventLoop.h"
#include <string>
#include "net/socket.h"
#include <memory>


class Channel;
class EventLoop;
class Socket;
class TCPConnection:noncopyable,public std::enable_shared_from_this<TCPConnection>
{
private:
    enum StateE
    {
        kDisconnected,
        kConnecting,
        kConnected,
        kDisconnecting
    };
    Buffer inputbuff_;
    Buffer outputbuff_;
    std::atomic_int state_;
    EventLoop *loop_;  //属于哪个Loop;
    const Address localAddr_;
    const Address peerAddr_;
    //std::unique_ptr<Channel> channel_;
    std::unique_ptr<Channel> channel_;
    std::unique_ptr<Socket> socket_;
 

    void handleRead(Timestamp receivetime);
    void handleWrite();
    void handleClose();
    void handleError();
    void sendInLoop(const void* message,size_t len);
    void sendInLoop(const std::string &message);
    void shutDownInLoop();

    void setState(StateE state){state_ = state;}

public:
    TCPConnection(EventLoop* loop,int sockfd,const Address& localaddr,const Address& peeraddr);
    ~TCPConnection();
    
    EventLoop* getLoop(){return loop_;}
    const Address& localAddress() const {return localAddr_;}
    const Address& peerAddress() const {return peerAddr_;}
    bool connected(){return kConnected == state_;}
    bool disconnected(){return kDisconnected == state_;}

    bool getTcpInfo(struct tcp_info*)const;
    string getTcpInfoString() const ;

    void send(Buffer* buf);
    void send(const std::string message);

    void shutdown(); //线程不安全
    
    Buffer* inputBuffer(){return &inputbuff_;}
    Buffer* outputBuffer(){return &outputbuff_;}

    void connectEstablished();
    void connectDestroyed();

    
};




#endif
