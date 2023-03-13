#include "net/Buffer.h"
#include <errno.h>
#include <unistd.h>
#include <sys/uio.h>

const char Buffer::kCRKF[] = "\r\n";

ssize_t Buffer::readFd(int fd,int *saveErron)
{
    char stackbuf[65536];//栈上开辟空间

    struct iovec vec[2];
    const size_t writeablebytes = writeable();

    //第一块缓冲区指向堆上开辟的可写空间
    vec[0].iov_base = begin()+writeIndex_;
    vec[0].iov_len = writeable();

    //第二块缓冲区指向栈上开辟的额外空间
    vec[1].iov_base = stackbuf;
    vec[1].iov_len = sizeof(stackbuf);

    //堆上开辟的可写空间大于栈上时，直接选取栈上
    const int iovec = (writeable()>sizeof(stackbuf))?1:2;

    const size_t n = readv(fd,vec,iovec);

    if(n<0)
    {
        *saveErron = errno;
    }
    else if (n<=writeable())
    {
        writeIndex_ +=n;
    }
    else
    {
       writeIndex_ = buf_.size();
       append(stackbuf,n-writeablebytes);
    }    
    return n;
}


//outputbuffer_.writeFd()将outputbuffer中的数据发送到FD的内核中
ssize_t Buffer::writeFd(int fd, int *saveErrno)
{
    ssize_t n = ::write(fd,peek(),readable());
    if(n<0)
    {
        *saveErrno = errno;
    }
    return n;
}