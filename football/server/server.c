#include "head.h"

int port = 6666;
char *conf = "./footballd.conf";

struct Map court;
struct Bpoint ball;  //球的位置
struct BallStatus ball_status;  //球的状态
struct Score score;
int repollfd, bepollfd;
struct User *rteam, *bteam;
int epollfd;   //主反应堆

void logout(int signum){
    struct ChatMsg msg;
    bzero(&msg,sizeof(msg));
    msg.type = CHAT_FIN;
    strcpy(msg.msg,"The server has logout!\n");
    for(int i = 0;i < MAX;i++){
        if(rteam[i].online){
            send(rteam[i].fd, (void *)&msg, sizeof(msg), 0);
        } 
        if(bteam[i].online){
            send(bteam[i].fd, (void *)&msg, sizeof(msg), 0);
        }
    }
    close(epollfd);
    exit(1);
}

int epollfd;   //主反应堆

void logout(int signum){
    struct ChatMsg msg;
	bzero(&msg,sizeof(msg));
	strcpy(msg.msg,"The server has logout!\n");
    msg.type = CHAT_FIN;
    send(epollfd, (void *)&msg, sizeof(msg), 0);
    close(epollfd);
    exit(1);
}

int main(int argc,char **argv){
    int opt;
    int listener;

    //分别为两队创建线程
    pthread_t red_t,blue_t;

    //解析参数argv
    while((opt = getopt(argc,argv,"p:")) != -1){
        switch(opt){
            case 'p':
                port = atoi(optarg);
                break;
            default:
                //本ali云设置为6666
                fprintf(stderr,"Usage : %s =-p port\n",argv[0]);
                exit(1);
        }
    }


    //暂时使用不上的变量
    //
    //判断一下配置文件的合法性
    /*
    if(!port) port = atoi(get_conf_value(conf,"PORT"));
    court.width = atoi(get_conf_value(conf,"COLS"));
    court.height = atoi(get_conf_value(conf,"LINES"));

    court.start.x = 3;
    court.start.y = 3;
    
    ball.x = court.width / 2;
    ball.y = court.height / 2;

    bzero(&ball_status,sizeof(ball_status));
    ball_status.who = -1;
    bzero(&score,sizeof(score));
    */
    //
    //
    //

    //创建udp监听套接字
    if((listener = socket_create_udp(port)) < 0){
        perror("socket_create_udp()");
        exit(1);
    }

    DBG(GREEN"INFO"NONE" : Server start On port %d.\n", port);

    //两个线程池所管理的用户群
    rteam = (struct User *)calloc(MAX, sizeof(struct User));
    bteam = (struct User *)calloc(MAX, sizeof(struct User));

    //创建3个反应堆
    epollfd = epoll_create(MAX * 2);
    repollfd = epoll_create(MAX);    
    bepollfd = epoll_create(MAX);

    if(epollfd < 0 || repollfd < 0 || bepollfd < 0){
        perror("epoll_create()");
        exit(1);
    }

    //初始化消息队列
    struct task_queue redQueue;
    struct task_queue blueQueue;

    task_queue_init(&redQueue,MAX,repollfd);
    task_queue_init(&blueQueue,MAX,bepollfd);

    //创建两个执行sub_reactor的线程
    pthread_create(&red_t,NULL, sub_reactor, (void *)&redQueue);
    pthread_create(&blue_t,NULL, sub_reactor, (void *)&blueQueue);

    //主反应堆
    struct epoll_event ev,events[MAX * 2];
    ev.events = EPOLLIN;
    ev.data.fd = listener;

    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, listener,&ev) < 0){
        perror("epoll_ctl");
        exit(1);
    }

    struct sockaddr_in client;
    bzero(&client,sizeof(client));
    socklen_t len = sizeof(client);
	
	signal(SIGINT,logout);

    signal(SIGINT,logout);

    while(1){
        DBG(YELLOW"Main Reactor"NONE" : Waiting for client\n");
        int nfds = epoll_wait(epollfd, events, MAX * 2, -1);
        if(nfds < 0){
            perror("epoll_wait()");
            exit(1);
        }
        for(int i = 0;i < nfds;i++){
            struct User user;
            char buff[512] = {0};
            if(events[i].data.fd == listener){
                //如果listener监听到一个新的用户加入，则创建新的用户
                DBG(L_GREEN"Listened one new user.\n"NONE);
                int new_fd = udp_accept(listener,&user);
                DBG(L_RED"new_fd"NONE" : %d\n",new_fd);
                if(new_fd > 0){
                    DBG(YELLOW"Try to Add New User to SubReactor\n"NONE);
                    add_to_sub_reactor(&user);
                }
                //先只收数据
                //recvfrom(listener, buff, sizeof(buff), 0, (struct sockaddr *)&client, &len);
                //DBG(RED"Recv"NONE" : <%s:%d> --> %s \n",inet_ntoa(client.sin_addr),client.sin_port,buff);
            }
        }
    }
    return 0;
}

