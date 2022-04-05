#ifndef CONDITION
#define CONDITION

#include <pthread.h>
#include <exception>
#include <iostream>

class cond{
public:
    cond(){
        if(pthread_cond_init(&mcond, NULL) != 0){
            throw std::exception();
        }
        printf("pthread cond init successed\n");
    }

    ~cond(){
        pthread_cond_destroy(&mcond);
    }

    bool wait(pthread_mutex_t* mutex){
        return pthread_cond_wait(&mcond, mutex) == 0;
    }

    bool timewait(pthread_mutex_t* mutex, struct timespec t){
        return pthread_cond_timedwait(&mcond, mutex, &t) == 0;
    }

    bool signal(){
        return pthread_cond_signal(&mcond) == 0;
    }

    bool broadcast(){
        return pthread_cond_broadcast(&mcond) == 0;
    }
private:
    pthread_cond_t mcond;
};


#endif

