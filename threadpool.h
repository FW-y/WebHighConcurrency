#ifndef THREADPOOL
#define THREADPOOL

#include <pthread.h>
#include <list>
#include "locker.h"

template<typename T>
class threadpool{
public:
    threadpool(int num = 8, int max_request = 100000);
    ~threadpool();
    bool append(T* request);
private:
    int thread_num;
    int m_max_requests;
    pthread_t* m_threads;
    std::list<T*> m_workdqueue;
    locker m_locker;
    sem m_queuestat;
    bool m_stop;

    static void* worker(void* arg);
    void run();
};

#endif