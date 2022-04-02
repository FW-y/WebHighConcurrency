#include "threadpool.h"
#include <cstdio>

template<typename T>
threadpool<T>::threadpool(int thread_number, int max_request)
    :thread_num(thread_number),m_max_requests(max_request),
    m_stop(false),m_threads(NULL)
{
    if(thread_number <= 0 || max_request <= 0)
        throw std::exception();
    
    m_threads = new pthread_t[thread_num];
    if(!m_threads){
        throw std::exception();
    }

    for(int i = 0; i < thread_num; ++i){
        printf("creat the %dth thread\n", i);        
        if(pthread_create(m_threads + i, NULL, worker, NULL) != 0){
            delete [] m_threads;
            throw std::exception();
        }
        if(pthread_detach(m_threads + i)){
            delete [] m_threads;
            throw std::exception();
        }
    }
}


template<typename T>
threadpool<T>::~threadpool(){
    delete[] m_threads;
    m_stop = true;
}

template<typename T>
bool threadpool<T>::append(T* request){
    m_locker.lock();
    if(m_workdqueue.size() > m_max_requests){
        m_locker.unlock();
        return false;
    }
    m_workdqueue.push_back(request);
    m_locker.unlock();
    m_queuestat.post();
    return true;
}

template<typename T>
void* threadpool<T>::worker(void* arg){
    threadpool* pool = (threadpool*)arg;
    pool->run();
    return pool;
}

template<typename T>
void threadpool<T>::run(){
    while(!m_stop){
        m_queuestat.wait();
        m_locker.lock();
        if(m_workdqueue.empty()){
            m_locker.unlock();
            continue;
        }

        T* request = m_workdqueue.front();
        m_workdqueue.pop_front();
        m_locker.unlock();

        if(!request){
            continue;
        }

        request->process();
    }
}