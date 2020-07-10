#include "head.h"

int main(int argc, char *argv[]) {
    char ip[20] = {0};
    int port = 0, sockfd;
    char buff[512] = {0};
    struct sockaddr_in server;
    if (argc != 3) {
        fprintf(stderr, "Usage: %s ip port!\n", argv[0]);
        exit(1);
    }
    strcpy(ip, argv[1]);
    port = atoi(argv[2]);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect");
        exit(1);
    }

    //sprintf(buff, "%s is always %d years old!", "suyelu", 18);
    char num[] = "2019302141199\0";
    char name[] = "陈树文\0";
    int i = 0;
    int j = 0;
    if(send(sockfd,num,strlen(num) + 1,0)  < 0){
        perror("send");
        exit(1);
    }
    // while(j<=10){
    //     if(recv(sockfd,&i,sizeof(int),0) == 0){
    //         close(sockfd);
    //     }
    //     //printf("%s\n",buff);
    //     //int a = atoi(buff);
    //     printf("%d",i);
    //     switch(i){
    //         case 1:
    //             send(sockfd,num,strlen(num) + 1,0);
    //             break;
    //         case 2:
    //             send(sockfd,name,strlen(name) + 1,0);
    //             break;
    //         case 3:
    //             close(sockfd);
    //             return 0;
    //         default:
    //             break;
    //     }
    //     j++;
    // }
    //send(sockfd, buff, strlen(buff), 0);
    
    close(sockfd);
}
