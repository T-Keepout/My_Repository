#include <65func.h>
#include "threadPool.h"
int exitPipe[2];//handler只能访问全局的数据
void handler(int signum){
    printf("Parent got signal, signum = %d\n", signum);
    write(exitPipe[1],"1",1);
}
int main(int argc, char *argv[])
{
    // ./server 192.168.72.128 12345 3
    ARGS_CHECK(argc,4);
    pipe(exitPipe);
    if(fork()){
        close(exitPipe[0]); // 父进程关闭管道的读端
        signal(SIGUSR1,handler);
        wait(NULL);
        printf("Parent is going to exit!\n");
        exit(0);
    }
    close(exitPipe[1]);
    threadPool_t threadPool;
    //初始化数据结构
    int workerNumber = atoi(argv[3]);
    threadPoolInit(&threadPool, workerNumber);
    //创建若干个子线程，修改tidArr的内容
    makeWorker(&threadPool);
    //tcp
    int sockfd = tcpInit(argv[1],argv[2]);
    //epoll
    int epfd = epoll_create(1);
    epollAdd(epfd,sockfd);
    epollAdd(epfd,exitPipe[0]);
    
    struct epoll_event readyset[1024];
    while(1){
        int readynum = epoll_wait(epfd,readyset,1024,-1);
        for(int i = 0; i < readynum; ++i){
            if(readyset[i].data.fd == sockfd){
                int netfd = accept(sockfd,NULL,NULL);
                //入队的第一步，是加锁
                pthread_mutex_lock(&threadPool.mutex);
                printf("I am master, I got a netfd = %d\n", netfd);
                enQueue(&threadPool.taskQueue,netfd);//入队
                pthread_cond_broadcast(&threadPool.cond);//唤醒所有工人
                pthread_mutex_unlock(&threadPool.mutex);
            }
            else if(readyset[i].data.fd == exitPipe[0]){
                printf("threadPool is going to exit!\n");
                //for(int j = 0; j < workerNumber; ++j){
                //    pthread_cancel(threadPool.tidArr.arr[j]);
                //}
                // 修改共享的exitFlag && 唤醒子线程
                pthread_mutex_lock(&threadPool.mutex);
                threadPool.exitFlag = 1;
                pthread_cond_broadcast(&threadPool.cond);
                pthread_mutex_unlock(&threadPool.mutex);

                for(int j = 0; j < workerNumber; ++j){
                    pthread_join(threadPool.tidArr.arr[j],NULL);
                }
                printf("Master is going to exit!\n");
                exit(0);
            }
        }
    }
    return 0;
}

