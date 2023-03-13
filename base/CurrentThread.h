#ifndef BASE_CURRENT_THREAD
#define BASE_CURRENT_THREAD

#include <unistd.h>
#include<sys/syscall.h>


namespace CurrentThread
{
    extern __thread int t_cachedTid;

    void cacheTid();

    inline int tid()
    {
        if(__builtin_expect(t_cachedTid==0,0))//"允许程序员将最有可能执行的分支告诉编译器"。这个指令的写法为：__builtin_expect(EXP, N)。意思是：EXP==N的概率很大。
        {
            cacheTid();
        }
        return t_cachedTid;
    }

}





#endif


