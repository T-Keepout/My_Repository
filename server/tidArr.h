#ifndef __TID_ARR__
#define __TID_ARR__
#include <65func.h>
typedef struct tidArr_s {
    pthread_t *arr;//tid数组的首地址
    int workerNum;//数组的长度
} tidArr_t;
int tidArrInit(tidArr_t *ptidArr, int workerNum);
#endif
