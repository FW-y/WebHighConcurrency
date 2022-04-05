#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>


int main(){
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1){
        perror("socket");
        exit(-1);
    }
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    inet_pton(AF_INET, "192.168.81.131", &saddr.sin_addr.s_addr);
    saddr.sin_port = htons(9999);
    connect(lfd, (struct sockaddr*)&saddr, sizeof(saddr));

    char mes[] = "hi, im cilent\n";
    write(lfd, mes, sizeof(mes));
    
    char recvBuf[1024] = {0};
    int ret = read(lfd, recvBuf, sizeof(recvBuf));
    printf("sever msg: %s\n", recvBuf);
    char sendmsg[1024] = {0};
    int i = 1;
    while(sendmsg[0] != 'q'){
        printf("%dth send\n", i);
        gets(sendmsg);
        write(lfd, sendmsg, strlen(sendmsg));
    }
    close(lfd);
    return 0;
}