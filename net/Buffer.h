#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <string>
#include <algorithm>

class Buffer
{
public:
    
    static const int BufSize = 1024;

    Buffer();
    ~Buffer();

    int readable() const {return writeIndex_-readIndex_;}
    int writeable()const {return end()-writeIndex_;}
    int pretendable()const {return readIndex_;}

    //缓冲区可读起始地址
    const char* peek() const {return begin()+readIndex_;}

    void retreive(size_t len)
    {
        if(len<readable())
        {
            readIndex_+=len;
        }
        else
        {
            retreiveAll();
        }       
    }

    void retreiveAll()
    {
        readIndex_ = 0;
        writeIndex_ = 0;
    }

    void retreiveUntil(const char* end)
    {
        retreive(end-peek());
    }
    
    std::string retreiveAsString(size_t len)
    {
        std:: string result(peek(),len);
        retreive(len);
        return result;
    }

    std::string retreiveAllAsString()
    {
        std::string result = retreiveAsString(readable());
        return result;
    }

    void ensureWriteSpace(size_t len)
    {
        if(len>writeable())
        {
            makespace(len);
        }
    }

    void append(const std::string &str)
    {
        append(str.data(),str.size());
    }

    void append(const char* data,size_t len)
    {
        ensureWriteSpace(len);
        std::copy(data,data+len,beginWrite());
        writeIndex_+=len;
    }

    const char* beginWrite() const
    {
        return begin()+writeIndex_;
    }

    char* beginWrite()
    {
        return begin()+writeIndex_;
    }

    const char*findCRLF()const
    {
        const char* crlf = std::search(peek(),beginWrite(),kCRKF,kCRKF+2);
        return crlf == beginWrite()? nullptr:crlf;
    }

    ssize_t readFd(int fd,int *saveErrno);
    
    ssize_t writeFd(int fd,int *saveErrno);

private:
    int readIndex_;
    int writeIndex_;
    std::vector<char> buf_;
    static const char kCRKF[];
    
    int end() const {return (int)buf_.size();}
    
    char* begin()
    {
        return &(*buf_.begin());
    }

    const char* begin() const 
    {
        return &(*buf_.begin());
    }

    void makespace(size_t len)  //声明成私有成员函数，防止外部使用
    {
        if(writeable()+pretendable()<len)
        {
            buf_.resize(writeIndex_+len);
        }
        else
        {
            int readablebytes = readable();
            std::copy(begin()+readIndex_,begin()+writeIndex_,begin());
            readIndex_ = 0;
            writeIndex_ = readIndex_+readablebytes;           
        }
        
    }

};


#endif