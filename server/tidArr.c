#include "tidArr.h"
int tidArrInit(tidArr_t *ptidArr, int workerNum){
    ptidArr->arr = (pthread_t *)calloc(workerNum,sizeof(pthread_t));
    ptidArr->workerNum = workerNum;
    return 0;
}
