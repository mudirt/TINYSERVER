#ifndef NET_ADDRESS_H
#define NET_ADDRESS_H

#include "base/types.h"
#include <netinet/in.h>
#include<iostream>
#include "base/copyable.h"
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

class Address: public copyable
{
public:


    explicit Address(uint16_t port, bool loopbackOnly = false);

    explicit Address(const char* ip, uint16_t port);

    explicit Address(const Address& addr):addr_(addr.addr_)//传入参数的const属性需一致
    {
    }

    explicit Address(const struct sockaddr_in addr):addr_(addr)
    {

    }

    const sockaddr_in*  getSocketAddress()const{return &addr_;}

    void setSocketAddress(const sockaddr_in& addr){addr_ = addr;}

    sa_family_t family() const { return addr_.sin_family; }
    
    string toIp() const;
    string toIpPort() const;
    uint16_t port() const;
    

private:
    sockaddr_in addr_;

};

#endif