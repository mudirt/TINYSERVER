#ifndef BASE_THREAD_H
#define BASE_THREAD_H

#include <thread>
#include "noncopyable.h"
#include <functional>

class Thread: noncopyable
{
public:
    using ThreadFunc = std::function<void()>;
    explicit Thread(ThreadFunc);
    ~Thread();

    void start();
    void join();

    bool started() const {return started_;}
    pid_t tid() const {return tid_;}

private:
    bool started_;
    bool joined_;
    std::shared_ptr<std::thread> thread_;
    pid_t tid_;

    ThreadFunc func_;

};




#endif