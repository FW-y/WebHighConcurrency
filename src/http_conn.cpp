#include "../include/http/http_conn.h"

int http_conn::EpollFd      = -1;
int http_conn::UserConut    = 0;

void setnonblocking(int fd){
    int old_flg = fcntl(fd, F_GETFL);
    int new_flg = old_flg | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_flg);

}

void addfd(int epollfd, int fd, bool oneshot){
    epoll_event event;
    event.data.fd = fd;
    // event.events = EPOLLIN | EPOLLHUP;
    event.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
    if(oneshot){
        event.events | EPOLLONESHOT;
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);

    setnonblocking(fd);
}

void rmfd(int epollfd, int fd){
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

void modfd(int epollfd, int fd, int ev){
    epoll_event event;
    event.data.fd = fd;
    event.events = ev | EPOLLIN | EPOLLHUP;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

void http_conn::init(int sockfd, const sockaddr_in& addr){
    printf("try to http_conn init\n");
    SocketFd    = sockfd;
    Socketaddr  = addr;
    std::cout << "client ip: ";
    char clientip[16];
    std::cout <<inet_ntop(AF_INET, &Socketaddr.sin_addr.s_addr,clientip, sizeof(clientip));
    std::cout << "client port: ";
    std::cout << ntohs(Socketaddr.sin_port) << std::endl;

    printf("try to http port reuse\n");

    int reuse = 1;
    int ret = setsockopt(SocketFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if(ret == -1){
        perror("setsockopt");
        exit(-1);
    }
    addfd(EpollFd, SocketFd, true);
    printf("port reuse successed\n");
    process_init();
    ++UserConut;
}

void http_conn::close_conn(){
    if(SocketFd!= -1){
        rmfd(EpollFd, SocketFd);
        SocketFd = -1;
        --UserConut;
    }
}

bool http_conn::read(){
    printf("reading..\n");
    if(ReadIdx >= READ_BUFFER_SIZE){
        printf("readbuf overflow..\n");
        return(false);
    }

    int readbytes = 0;
    while(true){
        printf("read begin\n");
        readbytes = recv(SocketFd, ReadBuf+ReadIdx, READ_BUFFER_SIZE-ReadIdx, 0);
            printf("readbytes: %d\n", readbytes);
        if(readbytes == -1){
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                // no date
                printf("read -1\n");
                break;
            }
            return false;
        }else if (readbytes == 0){
            // client closed
            printf("read fd close\n");
            return false;
        }
        printf("read success");
        ReadIdx += readbytes;
    }
    printf("read data : %s\n", ReadBuf);
    return true;
}

void http_conn::process_init(){
    CheckIdx = 0;
    CheckState = CHECK_STATE_REQUESTLINE;
    LineStartIdx = 0;

    Method = GET;
    Url = nullptr;
    Version = nullptr;
    Host = nullptr;
    Linger = false;

    setbzero();
}

http_conn::HTTP_CODE http_conn::process_read(){
    LINE_STATUS line_status = LINE_OK;
    HTTP_CODE ret = NO_REQUEST;

    char* text = NULL;

    while(  ((CheckState == CHECK_STATE_CONTENT) && (line_status == LINE_OK)) ||
            (line_status = parse_line(text)) == LINE_OK){
        text = get_line();
        LineStartIdx = CheckIdx;
        printf("got 1 http line : %s\n",text);

        switch (CheckState)
        {
        case CHECK_STATE_REQUESTLINE:{
            ret = parse_request_line(text);
            if(ret == BAD_REQUEST){
                return BAD_REQUEST;
            }
            break;
        }
        case CHECK_STATE_HEADER:{
            ret = parse_headers(text);
            if(ret == BAD_REQUEST){
                return BAD_REQUEST;
            }else if(ret == GET_REQUEST){
                return do_request();
            }
            break;
        }
        case CHECK_STATE_CONTENT:{
            ret = parse_content(text);
            if(ret == BAD_REQUEST){
                return BAD_REQUEST;
            }else if(ret == GET_REQUEST){
                return do_request();
            }
            line_status = LINE_OPEN;
            break;
        }
        
        default: return INTERNAL_ERROR;
            break;
        }
    }
    return NO_REQUEST;
}

http_conn::HTTP_CODE http_conn::parse_request_line(char *text){

}

http_conn::HTTP_CODE http_conn::parse_headers(char *text){

}

http_conn::HTTP_CODE http_conn::parse_content(char *text){

}

http_conn::LINE_STATUS http_conn::parse_line(char *text){
    char temp;

    for(; CheckIdx < ReadIdx; ++CheckIdx){
        temp = ReadBuf[CheckIdx];
        if(temp == '\r'){
            if(ReadIdx == CheckIdx+1){
                return LINE_OPEN;
            }else if(ReadBuf[CheckIdx+1] == '\n'){
                ReadBuf[CheckIdx++] = '\0';
                ReadBuf[CheckIdx++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }else if(temp == '\n'){
            if((CheckIdx > 1) && (ReadBuf[CheckIdx-1] == '\r')){
                ReadBuf[CheckIdx-1] = '\0';
                ReadBuf[CheckIdx++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
    }
    return LINE_OPEN;
}

http_conn::HTTP_CODE http_conn::do_request(){

}

bool http_conn::write(){
    printf("write..\n");
    return true;
}

void http_conn::process(){
    HTTP_CODE read_ret = process_read();
    if(read_ret == NO_REQUEST){
        modfd(EpollFd, SocketFd, EPOLLIN);
        return;
    }
    printf("process..\n");
}


