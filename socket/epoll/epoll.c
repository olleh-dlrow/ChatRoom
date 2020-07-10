#include "head.h"
#include "common.h"
#define MAX_EVENTS 25
#define MAX_CLIENTS 250
#define BUF_SIZE 4096


int main(int argc, char **argv) {
    int epollfd, server_listen, fd, port;
    char ip[20] = {0}, buff[BUF_SIZE] = {0};
    //初始化处理事件
    struct epoll_event ev, events[MAX_EVENTS];
    if (argc != 2) {
        fprintf(stderr, "Usage : %s port!\n", argv[0]);
        exit(1);
    }
    port = atoi(argv[1]);

    //创建监听套接字
    if ((server_listen = socket_create(port)) < 0) {
        perror("socket_createt()");
        exit(1);
    }

    //设置非阻塞
    make_non_block(server_listen);

    //创建专属的epoll套接字
    if ((epollfd = epoll_create(MAX_CLIENTS)) < 0) {
        perror("epoll_create()");
        exit(1);
    }

    //添加监听事件
    ev.events = EPOLLIN;          //可读事件类型
    ev.data.fd = server_listen;

    //注册事件
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_listen, &ev) < 0) {
        perror("epoll_ctl");
        exit(1);
    }
    //开始接收各种事件
    for (;;) {
        int nfds;
        //-1相当于阻塞，返回发生事件数nfds
        if ((nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1)) < 0) {
            perror("epoll_wait()");
            exit(1);
        }
        //处理每一个事件
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == server_listen) {
                //client结构体用来存放客户端的地址
                struct sockaddr_in client;
                socklen_t len = sizeof(client);
                //accept返回一个包含客户端信息的fd
                if ((fd = accept(server_listen, (struct sockaddr *)&client, &len)) < 0) {
                    perror("accept()");
                    exit(1);
                }
                make_non_block(fd);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = fd;

                //将该fd添加（注册）到轮询事件当中
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
                    perror("epoll_ctl()");
                    exit(1);
                }
            } else {
                //如果当前事件有内容并且为 可读 | 打断 | 错误 事件时 进行以下处理
                if (events[i].events & (EPOLLIN | EPOLLHUP | EPOLLERR)) {
                    memset(buff, 0, sizeof(buff));
                    if (recv(events[i].data.fd, buff, BUF_SIZE, 0) > 0) {
                        printf("Recv: %s\n", buff);
                        for (int i = 0; i < strlen(buff); i++) {
                            if (buff[i] >= 'a' && buff[i] <= 'z') buff[i] -= 32;
                        }
                        send(events[i].data.fd, buff, strlen(buff), 0);
                        //如果没有接收到消息或者错误，删除该事件
                    } else {
                        if (epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, NULL) < 0) {
                            perror("epoll_ctl()");
                            exit(1);
                        }
                        //表示一个人退出了
                        close(events[i].data.fd);
                    }
                }
            }
        }
    }

    return 0;
}