/*************************************************************************
	> File Name: server.c
	> Author: suyelu
	> Mail: suyelu@126.com
	> Created Time: Tue 07 Jul 2020 03:34:57 PM CST
 ************************************************************************/

#include "head.h"
#include "color.h"
#include "common.h"

#define MAX_CLIENT 250
#define BUF_SIZE 4096

//储存客户端信息
struct Buffer{
    int fd;
    char buff[BUF_SIZE];
    int flag;               //= 1 表示该消息待发送
    int sendindex;
    int recvindex;
};


struct Buffer *AllocBuffer(){
    struct Buffer *buffer = (struct Buffer *)malloc(sizeof(struct Buffer));
    if (buffer == NULL) {
        return NULL;
    }
    buffer->fd = -1;
    buffer->flag = buffer->recvindex = buffer->sendindex = 0;
    memset(buffer->buff, 0, BUF_SIZE);
    return buffer;
}

void FreeBuffer(struct Buffer *buffer) {
    free(buffer);
}

char ch_char(char c) {
    if (c >= 'a' && c <= 'z')
        return c - 32;
    return c;
}

int RecvToBuffer(int fd, struct Buffer *buffer) {
    char ip[20] = {0};
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    //fd中存有四元组，可以获得客户的ip和port
    getpeername(fd, (struct sockaddr *)&client, &len);
    strcpy(ip, inet_ntoa(client.sin_addr));
    //初始化收到消息个数为-1
    int nrecv = -1;
    while (1) {
        char buff[BUF_SIZE] = {'\0'};
        nrecv = recv(fd, buff, sizeof(buff), 0);
        printf("%s --> %s.",ip,buff);
        //未读取到(0)或者读取错误，就退出死循环
        if (nrecv <= 0) break;
        DBG("<"GREEN"Recv Data"NONE"> : %s --> %s.\n", ip, buff);
        //对获得的每一条消息进行处理
        for (int i = 0; i < nrecv; i++) {
            if (buffer->recvindex < sizeof(buffer->buff))
                buffer->buff[buffer->recvindex++] = ch_char(buff[i]);
            //condition?
            //可以在下次循环中发送了！
            buffer->flag = 1;
        }
        DBG("<"GREEN"Recv Data"NONE"> : %s --> %s.\n", ip, buffer->buff);
    }
    if (nrecv < 0) {
        //EAGAIN：套接字已标记为非阻塞，而接收操作被阻塞或者接收超时
        if (errno == EAGAIN)
            return 0;
        return -1;
    } else if (nrecv == 0) {    //此时不再接收到任何消息
        return 1;
    }
}


int SendFromBuffer(int fd, struct Buffer *buffer) {
    char ip[20] = {0};
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    getpeername(fd, (struct sockaddr *)&client, &len);
    strcpy(ip, inet_ntoa(client.sin_addr));
    int nsend = -1;
    while (buffer->sendindex < buffer->recvindex) {
        DBG("<"L_GREEN"Send Data"NONE"> : send data to %s.\n", ip);
        nsend = send(fd, buffer->buff + buffer->sendindex, buffer->recvindex - buffer->sendindex, 0);
        if (nsend < 0) {
            if (errno == EAGAIN)
                return 0;
            buffer->fd = -1;
            return -1;
        }
        buffer->sendindex += nsend;
        if (buffer->sendindex == buffer->recvindex)
            buffer->sendindex = buffer->recvindex = 0;
        buffer->flag = 0;
        return 0;
    }
}



int main(int argc, char **argv) {
    if (argc != 2)  {
        fprintf(stderr, "Usage: %s port!\n", argv[0]);
        exit(1);
    }


    int server_listen, fd, max_fd, port;
    port = atoi(argv[1]);

    //创建监听套接字
    if ((server_listen = socket_create(port)) < 0) {
        perror("socket_create()");
        exit(1);
    }

    //创建可以监听的最大用户数目
    struct Buffer *buffer[MAX_CLIENT];

    //初始化，最初fd = -1
    for (int i = 0; i < MAX_CLIENT; i++) {
        buffer[i] = AllocBuffer();
    }

    make_non_block(server_listen);

    //套接字集合，用来存储待处理的文件类型
    fd_set rfds, wfds, efds;
    //找到其中的最大值，因为这是select 的第一个参数
    max_fd = server_listen;

    while (1) {
        //初始化
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_ZERO(&efds);

        //将监听套接字放入可读套接字集合中
        FD_SET(server_listen, &rfds);
        DBG("<"PINK"Add rfds"NONE"> : set %d in rfds.\n", server_listen);

        //遍历所有客户端套接字，分别处理读 写套接字
        for (int i = 0; i < MAX_CLIENT; i++) {
            if (buffer[i]->fd == server_listen) continue;
            if (buffer[i]->fd > 0) {
                if (max_fd < buffer[i]->fd) max_fd = buffer[i]->fd;
                FD_SET(buffer[i]->fd, &rfds);
                DBG("<"PINK"Add rfds"NONE"> : set %d in rfds.\n", buffer[i]->fd);
                if (buffer[i]->flag == 1) {
                    FD_SET(buffer[i]->fd, &wfds);
                    DBG("<"YELLOW"Add wfds"NONE"> : set %d in wfds.\n", buffer[i]->fd);
                }
            }
        }
        int ret_val;
        //select最后参数设成NULL表示阻塞（当收到消息时才返回）
        if ((ret_val = select(max_fd + 1, &rfds, &wfds, NULL, NULL)) < 0) {
            perror("select");
        }
        //表示有消息传递，等待消息处理
        DBG("<"BLUE"Select Return"NONE"> : select return success %d fds.\n", ret_val);

        //测试server_listen端是否有数据（是否有新的客户接入）
        if(FD_ISSET(server_listen, &rfds)) {
            DBG("<"L_RED"Connect Request"NONE"> : new client request for login.\n");
            struct sockaddr_in client;
            socklen_t len = sizeof(client);
            //接收消息
            if ((fd = accept(server_listen, (struct sockaddr *)&client, &len)) < 0) {
                perror("accept()");
                exit(1);
            }
            DBG("<"RED"Login Success"NONE"> : %s Login success.\n", inet_ntoa(client.sin_addr));
            if (fd > MAX_CLIENT) {
                fprintf(stderr, "too many clients!\n");
                close(fd);
            } else {
                make_non_block(fd);
                //如果fd还没有放入buffer中，放入
                if (buffer[fd]->fd == -1)
                    buffer[fd]->fd = fd;
            }
        }
        //对所有套接字进行处理
        for (int i = 0; i <= max_fd; i++) {
            int retval = 0;
            //不处理监听套接字和空buffer
            if (buffer[i]->fd == server_listen || buffer[i]->fd < 0) continue;
            if (FD_ISSET(buffer[i]->fd, &rfds)) {
                retval = RecvToBuffer(buffer[i]->fd, buffer[i]);
            }
            if (retval == 0 && FD_ISSET(buffer[i]->fd, &wfds)) {
                retval = SendFromBuffer(buffer[i]->fd, buffer[i]);
            }

            //如果连接断开（客户端不再发送消息时）
            if (retval) {
                struct sockaddr_in client;
                socklen_t len = sizeof(client);
                getpeername(buffer[i]->fd, (struct sockaddr *)&client, &len);
                DBG("<"RED"Logout"NONE"> : %s Logout.\n", inet_ntoa(client.sin_addr));
                buffer[i]->fd = -1;
                close(buffer[i]->fd);
            }
        }
    }
    return 0;
}