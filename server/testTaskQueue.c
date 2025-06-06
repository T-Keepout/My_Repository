#include "taskQueue.h"
static void printQueue(taskQueue_t *pqueue){
    node_t * pCur = pqueue->pFront;
    while(pCur){
        printf("%3d", pCur->netfd);
        pCur = pCur->pNext;
    }
    printf("\n");
}
int main(){
    taskQueue_t queue;
    taskQueueInit(&queue);
    for(int i = 1; i <= 10; ++i){
        enQueue(&queue,i);
        printQueue(&queue);
    }
    printf("-----------------------\n");
    for(int i = 1; i <= 9; ++i){
        deQueue(&queue);
        printQueue(&queue);
    }
}
