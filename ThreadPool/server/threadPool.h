#ifndef __THREAD_POOL__
#define __THREAD_POOL__
#include <66func.h>
#include "tidArr.h"
#include "taskQueue.h"
typedef struct threadPool_s {
    // 子线程的信息 
    tidArr_t tidArr;
    // 任务队列
    taskQueue_t taskQueue;
    // 锁
    pthread_mutex_t mutex;
    // 条件变量
    pthread_cond_t cond;
    // 退出标志
    int exitFlag;
} threadPool_t;
int threadPoolInit(threadPool_t *pthreadPool,int workerNum);
int makeWorker(threadPool_t *pthreadPool);
int tcpInit(char *ip, char *port);
int epollAdd(int epfd, int fd);
int epollDel(int epfd, int fd);
int transfile(int sockfd);
#endif
