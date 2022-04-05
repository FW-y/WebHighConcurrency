#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct Node
{
    int num;
    struct Node* next;
};

struct Node *head = NULL;
pthread_mutex_t mutex;
sem_t sem;

void* product(void *arg){
    printf("kaigong..\n");
    sleep(3);
    while(1){
        pthread_mutex_lock(&mutex);
        struct Node* newnode = (struct Node*)malloc(sizeof(struct Node));
        newnode->next = head;
        head = newnode;
        head->num = rand()%100;
        printf("add node, num = %d; tip:  %ld.\n", head->num, pthread_self());
        pthread_mutex_unlock(&mutex);
        sem_post(&sem);
        sleep(1);
    }
    return NULL;
}
void* client(void *arg){
    while(1){
        printf("kankan youmeiyou huo\n");
        sem_wait(&sem);
        printf("you\n");
        pthread_mutex_lock(&mutex);

        struct Node* curnode = head;
        head = head->next;
        printf("delet node, num = %d; tip:  %ld.\n", curnode->num, pthread_self());
        free(curnode);

        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
    
}
int main(){
    pthread_t ptids[5], ctids[5];
    pthread_mutex_init(&mutex, NULL);
    sem_init(&sem, 0, 0);
    for(int i = 0; i < 5; ++i){
        pthread_create(&ptids[i], NULL, product, NULL);
        pthread_create(&ctids[i], NULL, client, NULL);
        
        pthread_detach(ptids[i]);
        pthread_detach(ctids[i]);
    }

    while(1){
        sleep(10);
    }

    pthread_exit(NULL);
    pthread_mutex_destroy(&mutex);
    sem_destroy(&sem);

    return 0;
}