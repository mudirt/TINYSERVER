#include "net/Address.h""


Address::Address(uint16_t port, bool loopbackOnly)
{
    bzero(&addr_,sizeof(addr_));
    addr_.sin_family = AF_INET;
    in_addr_t ip = loopbackOnly? INADDR_LOOPBACK:INADDR_ANY;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = htonl(ip);

}


Address::Address(const char* ip, uint16_t port)
{
    bzero(&addr_,sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = ::htons(port);
    
    if(inet_pton(AF_INET,ip,&addr_.sin_addr)<=0)
    {
        std::cout<<"INET_PTON ERROR"<<std::endl;
    }
}

string Address::toIp() const
{
    char buf[64] = {0};
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
    return buf;
}

string Address::toIpPort() const
{
    char buf[64]={0};
    inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
    size_t end = ::strlen(buf);
    uint16_t port = ::ntohs(addr_.sin_port);
    sprintf(buf+end,":%u",port);
    return buf;
}

uint16_t Address::port() const
{
    uint16_t port = ::ntohs(addr_.sin_port);
    return port;
}


// int main()
// {
//     Address addr1("1.2.3.4",80);
//     std::cout<<addr1.port()<<std::endl;
//     return 0;
// }


