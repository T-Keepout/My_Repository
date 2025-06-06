#ifndef __TID_ARR__
#define __TID_ARR__
#include <66func.h>
typedef struct tidArr_s {
    int workerNum;
    pthread_t *arr; //创建完结构体变量之后，还要申请内存
    //pthread_t arr[100];
} tidArr_t;
int tidArrInit(tidArr_t * ptidArr, int workerNum);
#endif
