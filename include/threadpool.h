#ifndef THREADPOOL
#define THREADPOOL

#include <list>
#include "locker/sem.h"
#include "locker/mutex.h"
#include "http/http_conn.h"

template<typename T>
class threadpool{
public:
    threadpool(int thread_num = 4, int max_request = 10000);
    ~threadpool();
    static void* worker(void* arg);
    void run();
    bool append(T* request);
private:
    int ThreadNum;
    int MaxRequest;
    pthread_t* Threads;
    mutex   Mutex;
    sem     Sem;
    bool    Stop;
    std::list<T*> WorkQueue;

};


#endif

