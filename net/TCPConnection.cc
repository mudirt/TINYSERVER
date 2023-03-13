#include "net/TCPConnection.h"
#include "net/Channel.h"
#include <functional>
#include <netinet/tcp.h>

TCPConnection::TCPConnection(EventLoop* loop,
                            int sockfd,
                            const Address& localaddr,
                            const Address& peeraddr
                            )
    :loop_(loop),
    peerAddr_(peeraddr),
    localAddr_(localaddr),
    state_(kConnecting),
    socket_(new Socket(sockfd)),
    channel_(new Channel(loop,sockfd))
{

    channel_->setReadCallback(
        std::bind(&TCPConnection::handleRead,this,std::placeholders::_1));   
    channel_->setCloseCallback(
        std::bind(&TCPConnection::handleClose,this));
    channel_->setErrorCallback(
        std::bind(&TCPConnection::handleError,this));
    channel_->setWriteCallback(
        std::bind(&TCPConnection::handleWrite,this));
}


TCPConnection::~TCPConnection()
{
}



void TCPConnection::send(const std::string message)
{
    if(state_ == kConnected)
    {
        if(loop_->isInLoopThread())
        {
            sendInLoop(message);
        }
        else
        {
            void (TCPConnection:: *fp)(const std::string&) = &TCPConnection::sendInLoop;
            loop_->queueInLoop(bind(fp,this,message));
        }
    }

}

void TCPConnection::sendInLoop(const std::string &message)
{
    sendInLoop(message.data(),message.size());
}

void TCPConnection::sendInLoop(const void* data,size_t len)
{
    ssize_t nwrote = 0;
    size_t remaining = len;

    if(!channel_->isWriting()&&outputbuff_.readable()==0)
    {
        nwrote = ::write(channel_->fd(),data,len);
        if(nwrote>=0)
        {
            remaining = len-nwrote;
        }
        else //nwrote<0
        {
            nwrote = 0;
            std::cerr<<"sendInLoop Erro"<<std::endl;
        }
    }

    if(remaining>0)
    {
        outputbuff_.append((char*)data+nwrote,remaining);
        if(!channel_->isWriting())
        {
            channel_->enableWriting();
        }
    }
       
}

void TCPConnection::shutDownInLoop()
{
    if(!channel_->isWriting())
    {
        socket_->shutDownWrite();
    }
}

void TCPConnection::connectDestroyed()
{
    if (state_ == kConnected)
    {
        setState(kDisconnected);
        channel_->disableAll();
    }
    channel_->remove();
}

void TCPConnection::connectEstablished()
{
    setState(kConnected);

    channel_->tie(shared_from_this());
    
}

void TCPConnection::handleClose()
{
    setState(kDisconnected);
    channel_->disableAll();
}

void TCPConnection::handleError()
{
    //fix me
}

void TCPConnection::handleRead(Timestamp receivetime)
{
    int saveErrno = 0;
    ssize_t n = inputbuff_.readFd(channel_->fd(),&saveErrno);
    if(n==0)
    {
        handleClose();
    }
    else if(n<0)
    {
        errno = saveErrno;
        handleError();
    }
    else
    {
        //fix me;
    }
}

void TCPConnection::handleWrite()
{
    if(channel_->isWriting())
    {
        int saveErrno = 0;
        ssize_t n = outputbuff_.writeFd(channel_->fd(),&saveErrno);

        if(n>0)
        {
            outputbuff_.retreive(n);

            if(outputbuff_.readable()==0)//buffer可读数据读取完毕，关闭连接
            {
                channel_->disableWriting();

                if(state_ == kDisconnected)
                {
                    shutDownInLoop();
                }
            }
        }
        else
        {
            std::cerr<<"handlewrite() erro"<<std::endl;
        }
    }
    else
    {
        std::cerr<<"TCPConnection fd ="<<channel_->fd()<<"is down"<<std::endl;
    }
}



