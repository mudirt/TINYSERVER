#include "Thread.h"
#include <semaphore.h>
#include "CurrentThread.h"

Thread::Thread(ThreadFunc func) :
    started_(false), // 还未开始
    joined_(false),  // 还未设置等待线程
    tid_(0),         // 初始 tid 设置为0
    func_(std::move(func)) // EventLoopThread::threadFunc()
{    

}

Thread::~Thread()
{
    //线程启动时并未设置等待线程时才可调用
    if(started && !joined_)
    {
        thread_->detach();//设置线程分离，不需要等待线程结束，不会产生孤儿线程
    }
}


void Thread::start()
{
    started_ = true;
    sem_t sem;
    sem_init(&sem,false,0);

    thread_ = std::shared_ptr<std::thread> (new std::thread([&](){
        tid_ = CurrentThread::tid();
        sem_post(&sem); 
        func_();
    }));
    /**
     * 这里必须等待获取上面新创建的线程的tid
     * 未获取到信息则不会执行sem_post，所以会被阻塞住
     * 如果不使用信号量操作，则别的线程访问tid时候，可能上面的线程还没有获取到tid
     */
    sem_wait(&sem);
}

void Thread::join()
{
    joined_ = true;
    //等待线程执行完毕
    thread_->join();

}
