#include "head.h"
#include "common.h"

double scale = 0;

int send_int_data(int fd, int data) {
    if (send(fd, (void *)&data, sizeof(int), 0) < 0)
        return -1;
    return 0;
}

int recv_str_data(int fd, char *buff, ssize_t size){
    fd_set set;
    unsigned long ul = 1;
    struct timeval tm;
    tm.tv_sec = 1;
    tm.tv_usec = 0;
    int ret = 0;;
    ioctl(fd, FIONBIO, &ul);
    FD_ZERO(&set);
    FD_SET(fd, &set);
    if (select(fd + 1, &set, NULL, NULL, &tm) > 0) {
        ret = recv(fd, buff, size, 0);
    } else {
        ret =  -1;
    }
    ul = 0;
    ioctl(fd, FIONBIO, &ul);
    return ret;
}

void *work(void *arg) {
    pthread_t tid = pthread_self();
    pthread_detach(tid);
    int fd, score = 0;
    char name[50] = {0}, id[50] = {0}, tmp[20];
    fd = *(int *)arg;
    for (int i = 1; i <= 3; i++) {
        if (send_int_data(fd, i) < 0) {
            break;
        }
        if (i == 1) {
            if (recv_str_data(fd, id, sizeof(id)) > 0) {
                score += 70;
            }
        } else if (i == 2) {
            if (recv_str_data(fd, name, sizeof(name)) > 0) {
                score += 70;
            }
        } else if (i == 3) {
            if (recv_str_data(fd, tmp, sizeof(tmp)) == 0) {
                score += 60;
            }
        }

    }
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    getpeername(fd, (struct sockaddr *)&client, &len);
    printf("%s:%s:%s:%.2lf\n",inet_ntoa(client.sin_addr), id, name, score * scale);
    //store_score("test001", id, name, inet_ntoa(client.sin_addr), score * scale);
    close(fd);
    return NULL;
}


int main(int argc, char *argv[]){
    int server_listen, sockfd, tmp;

    if (argc != 2) {
        fprintf(stderr, "Usage : %s scale!\n", argv[0]);
        exit(1);
    }

    tmp = atoi(argv[1]);
    scale = tmp / 100.0;

    if ((server_listen = socket_create(8888)) < 0) {
        perror("socket_create()");
    }
    while (1) {
        pthread_t tid;
        if ((sockfd = accept(server_listen, NULL, NULL)) > 0)  {
            pthread_create(&tid, NULL, work, (void *)&sockfd);
        }
    }

    return 0;
}