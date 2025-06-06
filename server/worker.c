#include "threadPool.h"
//void unlock(void *arg){
//    threadPool_t * pthreadPool = (threadPool_t *)arg;
//    printf("unlock!\n");
//    pthread_mutex_unlock(&pthreadPool->mutex);
//}
//void *threadFunc(void *arg){
//    threadPool_t * pthreadPool = (threadPool_t *)arg;
//    while(1){
//        // 加锁
//        int netfd;
//        pthread_mutex_lock(&pthreadPool->mutex);
//        pthread_cleanup_push(unlock,pthreadPool);
//        while(pthreadPool->taskQueue.queueSize == 0){
//            pthread_cond_wait(&pthreadPool->cond, &pthreadPool->mutex);
//        }
//        netfd = pthreadPool->taskQueue.pFront->netfd;//获取队列队首
//        printf("I am worker, I got netfd = %d\n", netfd);
//        deQueue(&pthreadPool->taskQueue);
//        //pthread_mutex_unlock(&pthreadPool->mutex);
//        pthread_cleanup_pop(1);
//
//        printf("I am worker, do something!\n");
//        transfile(netfd);
//        close(netfd);
//    }
//}
void *threadFunc(void *arg){
    threadPool_t * pthreadPool = (threadPool_t *)arg;
    while(1){
        // 加锁
        int netfd;
        pthread_mutex_lock(&pthreadPool->mutex);
        while(pthreadPool->taskQueue.queueSize == 0 && pthreadPool->exitFlag == 0){
            pthread_cond_wait(&pthreadPool->cond, &pthreadPool->mutex);
        }
        if(pthreadPool->exitFlag == 1){
            printf("I am worker. I am going to exit!\n");
            pthread_mutex_unlock(&pthreadPool->mutex);
            pthread_exit(NULL);
        }
        netfd = pthreadPool->taskQueue.pFront->netfd;//获取队列队首
        printf("I am worker, I got netfd = %d\n", netfd);
        deQueue(&pthreadPool->taskQueue);
        pthread_mutex_unlock(&pthreadPool->mutex);

        printf("I am worker, do something!\n");
        transfile(netfd);
        close(netfd);
    }
}
int makeWorker(threadPool_t *pthreadPool){
    for(int i = 0; i < pthreadPool->tidArr.workerNum; ++i){
        pthread_create(&pthreadPool->tidArr.arr[i],NULL,threadFunc,pthreadPool);
    }
    return 0;
}
