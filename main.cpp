#include "threadpool.h"
#include "http_conn.h"
#include <stdio.h>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <signal.h>

#define MAX_FD 65535
#define MAX_EVENT_NUM 10000

void addsig(int sig, void(handler)(int)){
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = handler;
    sigfillset(&sa.sa_mask);
    sigaction(sig, &sa,NULL);
}

int main(int argc, char* argv[]){
    if(argc <= 1){
        printf("fellow that tip: %s port_number\n", basename(argv[0]));
        exit(-1);
    }

    int port = atoi(argv[1]);

    addsig(SIGPIPE, SIG_IGN);

    threadpool<http_conn> *pool = NULL;
    try{
        pool = new threadpool<http_conn>;
    }catch(...){
        exit(-1);
    }
    http_conn *users = new http_conn[MAX_FD];
    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    int reuse = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    struct  sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    bind(listenfd, (struct sockaddr*)&address, sizeof(address));

    listen(listenfd, 5);

    epoll_event events[MAX_EVENT_NUM];
    int epollfd = epoll_create(5);

    
}