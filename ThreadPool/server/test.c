#include <66func.h>
#include "taskQueue.h"
int main(){
    taskQueue_t queue;
    taskQueueInit(&queue);
    for(int i = 0; i < 10; ++i){
        enQueue(&queue,i);
        printQueue(&queue);
    }
    printf("------------------------\n");
    for(int i = 0; i < 9; ++i){
        deQueue(&queue);
        printQueue(&queue);
    }
}
