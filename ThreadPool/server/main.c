#include "threadPool.h"
int exitPipe[2];
void handler(int signum){
    printf("signum = %d\n", signum);
    write(exitPipe[1],"1",1);
}
int main(int argc, char *argv[])
{
    // ./server 192.168.72.128 12345 3
    pipe(exitPipe);
    if(fork()){
        close(exitPipe[0]);
        signal(SIGUSR1,handler);
        wait(NULL);
        printf("Parent is going to exit!\n");
        exit(0);
    }
    // 只有子进程能到这里
    close(exitPipe[1]);
    threadPool_t threadPool;
    int workerNum = atoi(argv[3]);
    threadPoolInit(&threadPool,workerNum);
    makeWorker(&threadPool);
    int sockfd = tcpInit(argv[1],argv[2]);
    int epfd = epoll_create(1);
    epollAdd(epfd,sockfd);
    epollAdd(epfd,exitPipe[0]);
    struct epoll_event readyset[1024];
    while(1){
        int readynum = epoll_wait(epfd,readyset,1024,-1);
        for(int i = 0; i < readynum; ++i){
            if(readyset[i].data.fd == sockfd){
                int netfd = accept(sockfd,NULL,NULL);
                printf("I am main. I got netfd = %d\n", netfd);
                //加锁
                pthread_mutex_lock(&threadPool.mutex);
                enQueue(&threadPool.taskQueue, netfd);
                pthread_cond_broadcast(&threadPool.cond);
                pthread_mutex_unlock(&threadPool.mutex);
            }
            else if(readyset[i].data.fd == exitPipe[0]){
                printf("threadPool is going to exit!\n");
                //for(int j = 0; j < workerNum; ++j){
                //    pthread_cancel(threadPool.tidArr.arr[j]);
                //}
                pthread_mutex_lock(&threadPool.mutex);
                threadPool.exitFlag = 1;
                pthread_cond_broadcast(&threadPool.cond);
                pthread_mutex_unlock(&threadPool.mutex);

                for(int j = 0; j < workerNum; ++j){
                    pthread_join(threadPool.tidArr.arr[j],NULL);
                }

                printf("Master is going to exit!\n");
                exit(0);
            }
        }
    }
    return 0;
}

