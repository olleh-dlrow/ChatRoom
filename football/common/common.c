#include "head.h"
#include "common.h"
extern char conf_ans[50];

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

    //不会报address already in use
    int reuse = 1;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(char*)&reuse,sizeof(int));

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
    ioctl(fd,FIONBIO,&ul);
}

void make_block(int fd){
    unsigned long ul = 0;
    ioctl(fd,FIONBIO,&ul);
}

char *get_conf_value(const char *path,const char *key){
    FILE *fp = NULL;
    ssize_t nrd;
    char *line = NULL, *sub = NULL;
    ssize_t len;

    if(path == NULL || key == NULL){
        fprintf(stderr,"Error in argument!\n");
        return NULL;
    }

    if((fp = fopen(path,"r")) == NULL){
        perror("fopen()");
        return NULL;
    }

    while((nrd = getline(&line, &len, fp)) != -1){
        if((sub = strstr(line,key)) == NULL){
            continue;
        }
        //printf("After strstr!\n");
        if(line[strlen(key)] == '='){
            //printf("Before strncpy!\n");
            int len_ans = nrd - strlen(key) - 2;
            strncpy(conf_ans, sub + strlen(key) + 1,len_ans);
            *(conf_ans + len_ans) = '\0';
            //printf("After strncpy!\n");
            break;
        }
    }
    free(line);
    fclose(fp);
    if(sub == NULL){
        return NULL;
    }
    return conf_ans;
}