#include <pthread.h>

#ifndef _THREAD_POLL_H
#define _THREAD_POLL_H

struct task_queue{
    int sum;
    int epollfd; //记录从反应堆
    struct User **team;
    int head;
    int tail;
    pthread_mutex_t mutex;    //枷锁

    //
    //pthread_cond_signal函数的作用是发送一个信号给另外一个正在处于阻塞等待状态的线程,
    //使其脱离阻塞状态,继续执行.如果没有线程处在阻塞等待状态,pthread_cond_signal也会成功返回。
    //
    //使用pthread_cond_signal不会有“惊群现象”产生，他最多只给一个线程发信号
    //根据各等待线程优先级的高低确定哪个线程接收到信号开始继续执行
    //
    pthread_cond_t cond;      //条件变量
};

void task_queue_init(struct task_queue *taskQueue, int sum, int epollfd);
void task_queue_push(struct task_queue *taskQueue, struct User *user);
struct User* task_queue_pop(struct task_queue *taskQueue);
void *thread_run(void *arg);

#endif