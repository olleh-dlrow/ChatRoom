#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define INS 5

int now = 0;
int sum = 0;
//添加互斥锁
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//多线程计算sum的值
void *add(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        if (now > 10000){
            pthread_mutex_unlock(&mutex);
            break;
        }
        sum += now;
        now++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t *tid;
    tid = calloc(INS, sizeof(pthread_t));
    for (int i = 0; i < INS; i++) {
        pthread_create(&tid[i], NULL, add, NULL);
    }
//
    for (int i = 0; i < INS; i++) {
        pthread_join(tid[i], NULL);
    }

    printf("sum = %d\n", sum);
    return 0;
}