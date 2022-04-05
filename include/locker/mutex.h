#ifndef MUTEX 
#define MUTEX 

#include "../c_head.h"
#include <exception>
#include <semaphore.h>

class mutex{
public:
    mutex(){
        if(pthread_mutex_init(&Mutex, NULL) != 0){
            throw std::exception();
        }
        printf("pthread mutex init successed\n");
    }

    ~mutex(){
        pthread_mutex_destroy(&Mutex);
    }

    bool lock(){
        printf("try to lock..\n");
        return pthread_mutex_lock(&Mutex) == 0;
    }

    bool unlock(){
        return pthread_mutex_unlock(&Mutex) == 0;
    }

    pthread_mutex_t* get(){
        return &Mutex;
    }
private:
    pthread_mutex_t Mutex;
};



#endif

