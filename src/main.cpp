#include "../include/http/http_conn.h"
#include "../include/threadpool.h"
#include <cstdio>
#include <iostream>

#define MAX_FD 65535
#define MAX_EVENT_NUM 10000

//添加信号捕捉
void addsig(int sig, void(handler)(int)){
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = handler;
    sigfillset(&sa.sa_mask);
    sigaction(sig, &sa,NULL);
}

extern void addfd(int epollfd, int fd, bool oneshot);
extern void rmfd(int epollfd, int fd);
extern void modfd(int epollfd, int fd, int ev);

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

    addfd(epollfd, listenfd, false);
    http_conn::EpollFd = epollfd;

    while(true){
        int num = epoll_wait(epollfd, events, MAX_EVENT_NUM, -1);
        if(num < 0 && errno != EINTR){
            std::cout << "epoll failure" << std::endl;
            break;
        }
        printf("epoll num = %d\n", num);
        for(int i = 0; i < num; ++i){
            int sockfd = events[i].data.fd;
            if(sockfd == listenfd){
                std::cout << "listenfd in.." << std::endl;
                struct sockaddr_in client_address;
                socklen_t client_addrlen = sizeof(client_address);
                int connfd = accept(listenfd, (struct sockaddr*)&client_address, &client_addrlen);
                std::cout << "lfd accept.." << std::endl;
                char clientip[16];
                std::cout << "client ip: ";
                std::cout <<inet_ntop(AF_INET, &client_address.sin_addr.s_addr,clientip, sizeof(clientip));
                std::cout << "client port: ";
                std::cout << ntohs(client_address.sin_port) << std::endl;

                if(http_conn::UserConut >= MAX_FD){
                    close(connfd);
                    continue;
                }
                users[connfd].init(connfd, client_address); 
                printf("listenfd all init successed\n");
            }
            else if(events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)){
                users[sockfd].close_conn();
            }else if(events[i].events & EPOLLIN){
                printf("try to read sockfd..\n");
                if(users[sockfd].read()){
                    pool->append(users + sockfd);
                }else users[sockfd].close_conn();
            }else if(events[i].events & EPOLLOUT){
                if(!users[sockfd].write()){
                    users[sockfd].close_conn();
                }
            }

        }
    }
    
}