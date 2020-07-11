#include "head.h"

extern int repollfd;
extern int bepollfd;

void do_work(struct User *user){
    //
    //收到一条消息，并打印。 
    char buff[512] = {0};
    DBG(L_GREEN"Begin Work"NONE"\n");
    struct ChatMsg msg;
    recv(user->fd, (void *)&msg, sizeof(msg), 0);
    if (msg.type & CHAT_WALL){
        printf("\n<%s> -WALL ~ %s \n\n",user->name, msg.msg);
        strcpy(msg.name,user->name);
        sendtoall(&msg);
    } else if(msg.type & CHAT_MSG){
        printf("\n<%s> -MSG ~ %s \n\n",user->name,msg.msg); 
        strcpy(msg.name,user->name);
        sendto1(&msg,user);
    } else if(msg.type & CHAT_FIN){
        user->online = 0;
        if(user->team)
            del_event(bepollfd,user->fd);
        else del_event(repollfd,user->fd);
        printf(GREEN"Server Info"NONE" : %s logout!\n\n",user->name);
        sprintf(buff,"小伙伴" YELLOW "%s" NONE "离开了会议室。\n",user->name);
        strcpy(msg.msg, buff);
        msg.type = CHAT_SYS;
        sendtoall(&msg);
        close(user->fd);
    } else if(msg.type & CHAT_FUNC){
        do_func(&msg,user);
    }
    //send(user->fd, buff, strlen(buff), 0);
    // DBG("In do_work %s\n",user->name);
}

void task_queue_init(struct task_queue *taskQueue, int sum, int epollfd){
    taskQueue->sum = sum;
    taskQueue->epollfd = epollfd;
    taskQueue->team = calloc(sum,sizeof(void *));
    taskQueue->head = taskQueue->tail = 0;
    pthread_mutex_init(&taskQueue->mutex,NULL);
    pthread_cond_init(&taskQueue->cond,NULL);
}

void task_queue_push(struct task_queue *taskQueue, struct User *user){
    pthread_mutex_lock(&taskQueue->mutex);
    taskQueue->team[taskQueue->tail] = user;
    DBG(L_GREEN"Thread Pool"NONE" : Task push %s\n", user->name);
    if(++taskQueue->tail == taskQueue->sum){
        DBG(L_GREEN"Thread Pool"NONE" : Task Queue End\n");
        taskQueue->tail = 0;
    }
    pthread_cond_signal(&taskQueue->cond);
    pthread_mutex_unlock(&taskQueue->mutex);
}

struct User* task_queue_pop(struct task_queue *taskQueue){
    pthread_mutex_lock(&taskQueue->mutex);
    while(taskQueue->tail == taskQueue->head){
        DBG(L_GREEN"Thread Pool"NONE" : Task Queue Empty, Waiting For Task\n");
        //如果未等到消息，暂时解锁，避免惊群效应
        pthread_cond_wait(&taskQueue->cond,&taskQueue->mutex);
    }
    struct User *user = taskQueue->team[taskQueue->head];
    DBG(L_GREEN"Thread Pool"NONE" : Task Pop %s\n",user->name);
    if(++taskQueue->head == taskQueue->sum){
        DBG(L_GREEN"Thread Pool"NONE" : Task Queue End\n");
        taskQueue->head = 0;
    }
    pthread_mutex_unlock(&taskQueue->mutex);
    return user;
}

void *thread_run(void *arg){
    pthread_detach(pthread_self());
    struct task_queue *taskQueue = (struct task_queue *)arg;
    while(1){
        struct User *user = task_queue_pop(taskQueue);
        DBG(L_GREEN"Thread Run"NONE" : Do Work Ready...\n");
        do_work(user);
    }
}


