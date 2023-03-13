#include "socket.h"
#include "unistd.h"
#include <netinet/tcp.h>
#include <sys/socket.h>
#include "Address.h"


Socket::~Socket()
{
    ::close(sockfd_);
}

int Socket::accept(Address *peeraddr)
{
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    ::memset(&addr,0,sizeof(addr));

    int connfd = ::accept4(sockfd_,(sockaddr*)&addr,&len,SOCK_NONBLOCK|SOCK_CLOEXEC);

    if(connfd>=0)
    {
        peeraddr->setSocketAddress(addr);
    }
    else
    {
        std::cerr<<"accept4 failde"<<std::endl;
    }
    return connfd;
}

void Socket::bind(const Address &localaddr)
{
    //const sockaddr_in& addr = localaddr.getSocketAddress();
    if(::bind(sockfd_,(sockaddr*)(&localaddr)->getSocketAddress(),static_cast<socklen_t>(sizeof(sockaddr_in)))!=0)//getsocketaddress不能返回引用，会强转失败
    {
        std::cerr<<"bind erro"<<std::endl;
    }
}

void Socket::listen()
{
    if(0 != ::listen(sockfd_,1024))
    {
        std::cerr<<"listen sockfd"<<sockfd_<<"failed"<<std::endl;
    }
}

void Socket::shutDownWrite()
{
    if(::shutdown(sockfd_,SHUT_WR)<0)
    {
        std::cerr<<"shutdownwrite failed"<<std::endl;
    }
}

void Socket::setKeepAlive(bool on)
{
    int opval = on? 1 : 0;
    ::setsockopt(sockfd_,SOL_SOCKET,SO_KEEPALIVE,&opval,static_cast<socklen_t>(sizeof opval));
}

void Socket::setReuseAddr(bool on)
{
    int opval = on? 1 : 0;
    ::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEADDR,&opval,static_cast<socklen_t>(sizeof opval));
}

void Socket::setTcpNoDelay(bool on)
{
    int opval = on? 1 : 0;
    ::setsockopt(sockfd_,IPPROTO_TCP,TCP_NODELAY,&opval,static_cast<socklen_t>(sizeof opval));
}

void Socket::setReusePort(bool on)
{
    int opval = on? 1 : 0;
    ::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEPORT,&opval,static_cast<socklen_t>(sizeof opval));
}
