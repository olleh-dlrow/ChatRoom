#include "head.h"
#include "common.h"

void *work(void *arg) {
    int fd;
    char buff[512] = {0};
    fd = *(int *)arg;
    recv(fd, buff, sizeof(buff), 0);
    printf("Recv Msg: %s \n", buff);
    close(fd);
    return NULL;
}


int main(){
    int server_listen, sockfd;
    if ((server_listen = socket_create(8888)) < 0) {
        perror("socket_create()");
    }
    while (1) {
        pthread_t tid;
        //产生新的socket套接字，用来与客户端互动
        if ((sockfd = accept(server_listen, NULL, NULL)) > 0)  {
            pthread_create(&tid, NULL, work, (void *)&sockfd);
        }
    }

    return 0;
}