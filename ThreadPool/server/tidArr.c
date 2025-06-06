#include "tidArr.h"
int tidArrInit(tidArr_t * ptidArr, int workerNum){
    ptidArr->workerNum = workerNum; 
    ptidArr->arr = (pthread_t *)malloc(workerNum * sizeof(pthread_t));
    return 0;
}
