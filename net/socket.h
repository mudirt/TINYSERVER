#ifndef SOCKET_H
#define SOCKET_H

class Address;

class Socket
{
public:
    explicit Socket(int sockfd)
        : sockfd_(sockfd)
    {

    }
    ~Socket();
    
    void listen();
    void bind(const Address &localAddress);
    int fd() const {return sockfd_;}
    int accept(Address *peeraddr);

    void shutDownWrite();//半关闭
    

    void setTcpNoDelay(bool on);    // 设置Nagel算法 
    void setReuseAddr(bool on);     // 设置地址复用
    void setReusePort(bool on);     // 设置端口复用
    void setKeepAlive(bool on);     // 设置长连接 


private:

    const int sockfd_;

};

#endif