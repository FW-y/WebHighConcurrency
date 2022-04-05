#ifndef HTTPCONNECTION
#define HTTPCONNECTION

#include "../c_head.h"
#include <iostream>
// #include <regex>


class  http_conn{
public:
    static int EpollFd;
    static int UserConut;
    static const int READ_BUFFER_SIZE = 2048;
    static const int WRITE_BUFFER_SIZE = 2048;

    //HTTP request meth
    enum METHOD{
        GET = 0, POST, HEAD, PUT, DELETE, TRACE, OPTIONS, CONNECT
    };
    // sever state
    enum CHECK_STATE{
        CHECK_STATE_REQUESTLINE = 0, CHECK_STATE_HEADER, CHECK_STATE_CONTENT
    };
    // LINE STATUS
    enum LINE_STATUS{
        LINE_OK = 0, LINE_BAD, LINE_OPEN
    };
    //baowenjiexi jieguo
    enum HTTP_CODE{
        NO_REQUEST = 0, GET_REQUEST, BAD_REQUEST, NO_RESOURCE,
        FORBIDDEN_REQUEST, FILE_REQUEST, INTERNAL_ERROR,CLOSED_CONNECTION
    };

    http_conn():SocketFd(-1),ReadIdx(0){}
    ~ http_conn(){}
    void init(int sockfd, const sockaddr_in& addr);
    void close_conn();
    bool read();
    bool write();
    void process();
private:
    int SocketFd;
    sockaddr_in Socketaddr;
    char ReadBuf[READ_BUFFER_SIZE];
    int ReadIdx;

    int CheckIdx; //buff checked index
    int LineStartIdx;

    char *Url; // request document name
    char *Version;
    METHOD Method;
    char *Host;
    bool Linger; //shifou baochi lianjie
    int ContentLength;

    CHECK_STATE CheckState;

    inline char* get_line(){return ReadBuf+LineStartIdx;}
    void process_init();
    HTTP_CODE process_read();
    HTTP_CODE parse_request_line(char * text);
    HTTP_CODE parse_headers(char * text);
    HTTP_CODE parse_content(char * text);

    HTTP_CODE do_request();

    LINE_STATUS parse_line(char * text);
};


#endif