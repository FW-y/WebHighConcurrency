#include "../include/threadpool.h"

template<typename T>
threadpool<T>::threadpool(int thread_number, int max_request):
    ThreadNum(thread_number),MaxRequest(max_request),
    Stop(false),Threads(NULL){
    if(thread_number <= 0 || max_request <= 0)
        throw std::exception();
    
    Threads = new pthread_t[ThreadNum];
    if(!Threads){
        throw std::exception();
    }

    for(int i = 0; i < ThreadNum; ++i){
        printf("creat the %dth thread\n", i);        
        if(pthread_create(Threads+ i, NULL, worker, this) != 0){
            delete [] Threads;
            throw std::exception();
        }
        if(pthread_detach(Threads[i])){
            printf("detach error\n");
            delete [] Threads;
            throw std::exception();
        }
    }
}

template<>
threadpool<http_conn>::threadpool(int thread_number, int max_request):
    ThreadNum(thread_number),MaxRequest(max_request),
    Stop(false),Threads(NULL){
    if(thread_number <= 0 || max_request <= 0)
        throw std::exception();
    
    Threads = new pthread_t[ThreadNum];
    if(!Threads){
        throw std::exception();
    }

    for(int i = 0; i < ThreadNum; ++i){
        printf("creat the %dth thread\n", i);        
        if(pthread_create(Threads+ i, NULL, worker, this) != 0){
            delete [] Threads;
            throw std::exception();
        }
        if(pthread_detach(Threads[i])){
            printf("detach error\n");
            delete [] Threads;
            throw std::exception();
        }
    }
}


template<typename T>
threadpool<T>::~threadpool(){
    delete[] Threads;
    Stop = true;
}

template<typename T>
bool threadpool<T>::append(T* request){
    Mutex.lock();
    if(WorkQueue.size() > MaxRequest){
        Mutex.unlock();
        return false;
    }
    WorkQueue.push_back(request);
    Mutex.unlock();
    Sem.post();
    return true;
}

template<>
bool threadpool<http_conn>::append(http_conn* request){
    Mutex.lock();
    if(WorkQueue.size() > MaxRequest){
        Mutex.unlock();
        return false;
    }
    WorkQueue.push_back(request);
    Mutex.unlock();
    Sem.post();
    return true;
}


template<typename T>
void* threadpool<T>::worker(void* arg){
    printf("try to work ..\n");
    threadpool* pool = (threadpool*)arg;
    pool->run();
    return pool;
}

template<typename T>
void threadpool<T>::run(){
    printf("working ..\n");
    while(true){
        if(Stop){
            printf("threadpool stop\n");
            break;
        } 
        Sem.wait();
        Mutex.lock();
        if(WorkQueue.empty()){
            Mutex.unlock();
            continue;
        }

        T* request = WorkQueue.front();
        WorkQueue.pop_front();
        Mutex.unlock();

        if(!request){
            continue;
        }

        request->process();
    }
}