#include "head.h"

extern struct  User *rteam,*bteam;

void sendtoall(struct ChatMsg *msg){
    int i;
    for(i = 0;i < MAX;i++){
        if(rteam[i].online){
            printf("Ready to send a msg to %s ,fd = %d\n",rteam[i].name,rteam[i].fd);
            send(rteam[i].fd,(void *)msg, sizeof(struct ChatMsg),0);
        }
        if(bteam[i].online){ 
            printf("Ready to send a msg to %s ,fd = %d\n",bteam[i].name,bteam[i].fd);
            send(bteam[i].fd,(void *)msg, sizeof(struct ChatMsg),0);
        }
    }
}


void sendto1(struct ChatMsg *msg,struct User *user){
    int len = strlen(msg->msg);
    int i;
    char buf[512] = {0};
    for(i = 1;i < len;i++){
        if(msg->msg[i] == ' ')break;
    }
    //如果@后面没有人名或者人名后面没有消息，就报错
    if(msg->msg[1] == ' ' || i == len){
        strcpy(buf,"The format is wrong!\n");
        printf("%s",buf);
        strcpy(msg->msg,buf);
        msg->type = CHAT_SYS;
        send(user->fd, (void *)msg, sizeof(struct ChatMsg),0);
        return;
    }
    
    char op_name[21];
    bzero(&op_name, sizeof(op_name));
    strncpy(op_name, msg->msg + 1, i - 1);
    op_name[i] = '\0';
    printf("op_name = %s\n",op_name);
    for(i = 0;i < MAX; i++){
        if(strcmp(rteam[i].name, op_name) == 0){
            if(!rteam[i].online){
                sprintf(buf,"%s is not online!\n",op_name);
                printf("%s",buf);
                strcpy(msg->msg,buf);
                msg->type = CHAT_SYS;
                send(user->fd, (void *)msg, sizeof(struct ChatMsg),0);
                return;
            } else {
				send(rteam[i].fd, (void *)msg, sizeof(struct ChatMsg),0);
				return;
            }
        }	
        if(strcmp(bteam[i].name, op_name) == 0){
            if(!bteam[i].online){
                sprintf(buf,"%s is not online\n",op_name);
                printf("%s",buf);
                strcpy(msg->msg,buf);
        		msg->type = CHAT_SYS;
                send(user->fd, (void *)msg, sizeof(struct ChatMsg),0);
                return;
            } else {
				send(bteam[i].fd, (void *)msg, sizeof(struct ChatMsg),0);
				return;
            }
        }
    }
	//没有找到该名字
	sprintf(buf,"not find name %s!\n",op_name);
	printf("%s",buf);
	strcpy(msg->msg,buf);
    msg->type = CHAT_SYS;
	send(user->fd,(void *)msg,sizeof(struct ChatMsg), 0);
	return;
}

#define FUNC_GETLIST 1

void do_func(struct ChatMsg *msg,struct User *user){
    char buf[512] = {0};
    int i;
    int len = strlen(msg->msg);
    for(i = 1;i < len;i++){
        if(msg->msg[i] == ' ')break;
    }
    if(i == 1){
        strcpy(buf,"The format is wrong!\n");
        msg->type = CHAT_SYS;
        strcpy(msg->msg,buf);
        send(user->fd, (void *)msg, sizeof(struct ChatMsg), 0);
        return;
    }
    else {
        strncpy(buf,msg->msg + 1, i - 1);
        buf[i] = '\0';
        int a = atoi(buf);
        printf("a = %d\n",a);
        if(a == FUNC_GETLIST){
            strcpy(buf,"Online List:\n");
            strcpy(msg->msg,buf);
            //send(user->fd,(void *)msg, sizeof(struct ChatMsg), 0);
            for(i = 0;i < MAX; i++){
                if(rteam[i].online){
                    sprintf(buf,"->%s\n",rteam[i].name);
                    strcat(msg->msg,buf);
                    //send(user->fd,(void *)msg, sizeof(struct ChatMsg), 0);
                } 
                if(bteam[i].online){
                    sprintf(buf,"->%s\n",bteam[i].name);
                    strcat(msg->msg,buf);
                    //send(user->fd,(void *)msg, sizeof(struct ChatMsg), 0);
                }
            }

            send(user->fd,(void *)msg, sizeof(struct ChatMsg), 0);
            return;
        }
    }
}


int socket_create_udp(int port){
    int server_listen;
    if((server_listen = socket(AF_INET,SOCK_DGRAM,0)) < 0){
        return -1;
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    setsockopt(server_listen, SOL_SOCKET, SO_REUSEADDR, &opt,sizeof(opt));
    make_non_block(server_listen);
    if (bind(server_listen,(struct sockaddr *)&server,sizeof(server)) < 0){
        return -1;
    }
    return server_listen;
}
