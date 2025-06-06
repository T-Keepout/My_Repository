#ifndef __THREAD_POOL___
#define __THREAD_POOL___
#include <65func.h>
#include "tidArr.h"
#include "taskQueue.h"
typedef struct threadPool_s {
    //工人线程的信息
    tidArr_t tidArr;
    //任务队列
    taskQueue_t taskQueue;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int exitFlag;//退出标志位
} threadPool_t;
int threadPoolInit(threadPool_t *pthreadPool,int workerNum);
int makeWorker(threadPool_t *pthreadPool);
int epollAdd(int epfd, int fd);
int epollDel(int epfd, int fd);
int tcpInit(char *ip, char *port);
int transfile(int sockfd);
#endif
