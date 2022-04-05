#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(){
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1){
        perror("socket");
        exit(-1);
    }
    struct sockaddr_in lsockaddr;
    lsockaddr.sin_family = AF_INET;
    //int haddr = inet_pton(AF_INET, "192.168. . ", lsockaddr.sin_addr.s_addr);
    lsockaddr.sin_addr.s_addr = INADDR_ANY; //0
    lsockaddr.sin_port = htons(9999);
    bind(lfd, (struct sockaddr*)&lsockaddr, sizeof(lsockaddr));

    printf("listening ..\n");
    listen(lfd, 8);

    struct sockaddr_in caddr;
    socklen_t caddrlen = sizeof(caddr);
    int cfd = accept(lfd, (struct sockaddr*)&caddr, &caddrlen);

    char cip[16];
    inet_ntop(AF_INET, &caddr.sin_addr.s_addr, cip, sizeof(cip));
    unsigned short cport = ntohs(caddr.sin_port);
    printf("clinet ip is %s, port is %d\n", cip, cport);

    char recvBuf[1024] = {0};
    while(1){
        int recvlen = read(cfd, recvBuf, sizeof(recvBuf));
        if(recvlen == -1){
            perror("read");
            exit(-1);
        }else if(recvlen > 0){
            printf("client msg: %s\n", recvBuf);
        }else if(recvlen == 0){
            printf("client close\n");
            close(cfd);
            break;
        }
        write(cfd, recvBuf, recvlen);
    }
    close(lfd);
    return 0;
}