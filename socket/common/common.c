#include "head.h"

//根据端口创建socket 描述字
int socket_create(int port) {
    //socket()
    //bind()
    //listen()
    int sockfd;
    struct sockaddr_in server;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }

    //协议族，一般使用ipv4的AF_INET
    server.sin_family = AF_INET;
    //将主机字节序转换成网络字节序
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        return -1;
    }

    if (listen(sockfd, 10) < 0) {
        return -1;
    }

    return sockfd;
}

void make_non_block(int fd){
    unsigned long ul = 1;
    //第二个参数代表设置(非0)或者清除（0）非阻塞标志：FIO NOT BLOCK IO
    ioctl(fd,FIONBIO,&ul);
}

void make_block(int fd){
    unsigned long ul = 0;
    ioctl(fd,FIONBIO,&ul);
}