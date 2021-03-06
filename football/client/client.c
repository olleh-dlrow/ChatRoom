#include "head.h"

int server_port = 0;
char server_ip[20] = {0};
char *conf = "./football.conf";
int sockfd = -1;

void logout(int signum){
    struct ChatMsg msg;
    msg.type = CHAT_FIN;
    send(sockfd, (void *)&msg, sizeof(msg), 0 );
    close(sockfd);
    exit(1);
}

int main(int argc, char **argv)
{
    //参数解析
    int opt;
    struct LogRequest request;
    struct LogResponse response;

    bzero(&request, sizeof(request));
    bzero(&response, sizeof(response));

    while ((opt = getopt(argc, argv, "h:p:t:m:n:")) != -1) {
        switch (opt) {
        case 't':
            request.team = atoi(optarg);
            break;
        case 'h':
            strcpy(server_ip, optarg);
            break;
        case 'p':
            server_port = atoi(optarg);
            break;
        case 'm':
            strcpy(request.msg, optarg);
            break;
        case 'n':
            strcpy(request.name, optarg);
            break;
        default:
            fprintf(stderr, "Usage : %s [-hptmn]!\n", argv[0]);
            exit(1);
        }
    }

    if (!server_port)
        server_port = atoi(get_conf_value(conf, "SERVERPORT"));

    if (!request.team)
        request.team = atoi(get_conf_value(conf, "TEAM"));

    if (!strlen(server_ip))
        strcpy(server_ip, get_conf_value(conf, "SERVERIP"));

    if (!strlen(request.name))
        strcpy(request.name, get_conf_value(conf, "NAME"));

    if (!strlen(request.msg))
        strcpy(request.msg, get_conf_value(conf, "LOGMSG"));

    DBG("<" GREEN "Conf Show" NONE
        "> : server_ip = %s, port = %d, team = %s, name = %s\n%s\n",
        server_ip, server_port, request.team ? "BLUE" : "RED", request.name, request.msg);

    //初始化服务端二元组
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    server.sin_addr.s_addr = inet_addr(server_ip);

    socklen_t len = sizeof(server);

    if ((sockfd = socket_udp()) < 0) {
        perror("socket_udp()");
        exit(1);
    }

    sendto(sockfd, (void *)&request, sizeof(request), 0,
           (struct sockaddr *)&server, len);

    //上一行已经发送request ,接下来等待response
    //使用select
    //实现，等待5秒，如果5秒内未收到数据，则判定为服务端不在线，退出程序

    fd_set rfds;

    int maxfd = sockfd;
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    //判断是否有文件就绪
    while (1) {
        FD_ZERO(&rfds);
        FD_SET(sockfd, &rfds);
        //返回就绪fd的数目
        int retval = select(maxfd + 1, &rfds, NULL, NULL, &tv);
        if (retval < 0) {
            perror("select()");
            exit(1);
        } else if (retval == 0) {
            DBG("<"RED"Login"NONE"> : Login Server Out of Time!\n");
            close(sockfd);
            return 0;
        } else {
            if (FD_ISSET(sockfd, &rfds)) {
                int ret;
                //接收信息到response
                ret = recvfrom(sockfd, (void *)&response, sizeof(response), 0,
                               (struct sockaddr *)&server, &len);
                if (ret < 0) {
                    perror("recvfrom");
                    exit(1);
                }
                //如果response大小不合法或者type ==
                //1（重复连接），则服务器拒绝接入，打印response.msg
                if (ret != sizeof(response) || response.type == 1) {
                    printf("%s\n", response.msg);
                    close(sockfd);
                    return 0;
                }
                //如果合理，则调用connect连接到服务端，相当于建立连接
                else {
                    printf("%s\n", response.msg);
                    if (connect(sockfd, (struct sockaddr *)&server,
                                sizeof(server)) < 0) {
                        perror("connect");
                        exit(1);
                    }
                }
                //发送一条消息给客户端，查看从反应堆的，do_work是否返回
                char buff[512] = {0};
                bzero(buff,sizeof(buff));
                // sprintf(buff,"%s","Hello!\n");
                // sendto(sockfd, &buff, strlen(buff), 0, (struct sockaddr *)&server, len);
                // bzero(buff,sizeof(buff));
                //recv(sockfd,buff,sizeof(buff),0);
                //DBG(RED"Server Info"NONE" : %s \n",buff);
                //
                //发送信号（当执行^C时，自动执行logout函数）
                signal(SIGINT, logout);
                while(1){
                    struct ChatMsg msg;
                    bzero(&msg,sizeof(msg));
                    msg.type = CHAT_WALL;
                    DBG(RED"Please Input: \n"NONE);
                    scanf("%[^\n]s",msg.msg);
                    getchar();
                    if(sendto(sockfd, (void *)&msg, sizeof(msg), 0, (struct sockaddr *)&server, len) < 0){
                        perror("send error");
                        exit(1);
                    }
                    make_non_block(sockfd);
                    if(recv(sockfd, (void *)&msg,sizeof(msg),0) <= 0){
                        continue;
                    } else {
                        DBG(L_GREEN"Sign From Server"NONE" : %s \n",msg.msg);
                        if(msg.type & CHAT_FIN){
                            close(sockfd);
                            return 0;
                        }
                    }
                    make_block(sockfd);
                }
            }
        }   
    }
    return 0;
}
