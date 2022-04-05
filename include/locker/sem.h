#ifndef SEM_H
#define SEM_H

#include "../c_head.h"
#include <exception>
#include <iostream>


class sem{
public:
    sem(int num = 0){
        printf("try to sen init, semnum = %d\n", num);
        if(sem_init(&m_sem, 0, num) != 0){
            throw std::exception();
        }
        printf("sem init success\n");
    }
    ~sem(){sem_destroy(&m_sem);}

    bool wait(){
        printf("sem try to dec..\n");
        return sem_wait(&m_sem) == 0;
    } 

    bool post(){return sem_post(&m_sem) == 0;}
private:
    sem_t m_sem;
};


#endif

