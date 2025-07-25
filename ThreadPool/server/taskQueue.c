#include "taskQueue.h"
int taskQueueInit(taskQueue_t *pqueue){
    bzero(pqueue,sizeof(taskQueue_t));    
    return 0;
}
int enQueue(taskQueue_t *pqueue, int netfd){
    node_t * pNew = (node_t *)calloc(1,sizeof(node_t));
    pNew->netfd = netfd;

    if(pqueue->queueSize == 0){
        pqueue->pFront = pNew;
        pqueue->pRear = pNew;
    }
    else{
        pqueue->pRear->pNext = pNew;
        pqueue->pRear = pNew;
    }
    ++pqueue->queueSize;
    return 0;
}
int deQueue(taskQueue_t *pqueue){
    node_t * pCur = pqueue->pFront;
    pqueue->pFront = pCur->pNext;
    if(pqueue->queueSize == 1){
        pqueue->pRear = NULL;
    }
    free(pCur);
    --pqueue->queueSize;
    return 0;
}
int printQueue(taskQueue_t *pqueue){
    node_t * pCur = pqueue->pFront;
    while(pCur){
        printf("%3d", pCur->netfd);
        pCur = pCur->pNext;
    }
    printf("\n");
    return 0;
}
