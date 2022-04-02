#ifndef LOCKER
#define LOCKER

#include <pthread.h>
#include <exception>
#include <semaphore.h>

class locker{
public:
    locker(){
        if(pthread_mutex_init(&mutex, NULL) != 0){
            throw std::exception();
        }
    }

    ~locker(){
        pthread_mutex_destroy(&mutex);
    }

    bool lock(){
        return pthread_mutex_lock(&mutex) == 0;
    }

    bool unlock(){
        return pthread_mutex_unlock(&mutex) == 0;
    }

    pthread_mutex_t* get(){
        return &mutex;
    }
private:
    pthread_mutex_t mutex;
};

class cond{
public:
    cond(){
        if(pthread_cond_init(&mcond, NULL) != 0){
            throw std::exception();
        }
    }

    ~cond(){
        if(pthread_cond_destroy(&mcond) != 0){
            throw std::exception();
        }
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

class sem{
public:
    sem(int num){
        if(sem_init(&m_sem, 0, num) != 0){
            throw std::exception();
        }
    }
    ~sem(){sem_destroy(&m_sem);}

    bool wait(){return sem_wait(&m_sem) == 0;} 

    bool post(){return sem_post(&m_sem) == 0;}
private:
    sem_t m_sem;
};

#endif