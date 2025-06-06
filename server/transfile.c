#include "threadPool.h"
typedef struct {
    int length;//火车头 4B
    char data[1000];//火车车厢 1000B是上限，不是每次的实际值
    //这里的char数组不是字符串的意思，只是为了方便控制长度 
}train_t;
// v1.0 发小文件
//int transfile(int sockfd){
//    char filename[] = "file1";
//    send(sockfd,filename,5,0); // 先发文件名
//    int fd = open(filename,O_RDWR);
//    char buf[1000] = {0};
//    ssize_t sret = read(fd,buf,sizeof(buf)); // 读磁盘文件内容
//    //send(sockfd,buf,strlen(buf),0); // 错误写法 磁盘文件不一定是文本文件
//    send(sockfd,buf,sret,0);// 正确写法
//    return 0;
//}
// v2.0 发小文件 用私有协议
//int transfile(int sockfd){
//    train_t train;
//    char filename[] = "file1";
//    train.length = 5;
//    memcpy(train.data,filename,train.length);
//    // send(sockfd,&train,sizeof(train),0); //错误 sizeof(train) 是长度上限不是实际长度
//    send(sockfd,&train,sizeof(train.length)+train.length, 0);
//
//    int fd = open(filename,O_RDWR);
//    ssize_t sret = read(fd,train.data,sizeof(train.data)); // 读磁盘文件内容
//    train.length = sret;
//    send(sockfd,&train,sizeof(train.length)+train.length, 0);
//    return 0;
//}
// v3.0 发大文件 用私有协议
//int transfile(int sockfd){
//    train_t train;
//    char filename[] = "file1";
//    train.length = 5;
//    memcpy(train.data,filename,train.length);
//    send(sockfd,&train,sizeof(train.length)+train.length, MSG_NOSIGNAL);
//
//    int fd = open(filename,O_RDWR);
//    while(1){
//        bzero(train.data,sizeof(train.data));
//        ssize_t sret = read(fd,train.data,sizeof(train.data)); // 读磁盘文件内容
//        train.length = sret;
//        send(sockfd,&train,sizeof(train.length)+train.length, MSG_NOSIGNAL);
//        if(sret == 0){ //把if写在send后面，最后一次会发空火车
//            break;
//        }
//    }
//    close(fd);
//    return 0;
//}
// v4.0 发大文件 用私有协议 发文件长度
//int transfile(int sockfd){
//    train_t train;
//    char filename[] = "file1";
//    train.length = 5;
//    memcpy(train.data,filename,train.length);
//    send(sockfd,&train,sizeof(train.length)+train.length, MSG_NOSIGNAL);
//
//    int fd = open(filename,O_RDWR);
//    struct stat statbuf;
//    fstat(fd,&statbuf);
//    off_t filesize = statbuf.st_size;//这个数据可以直接发，建议放入小火车
//    train.length = sizeof(filesize);
//    memcpy(train.data,&filesize,train.length);
//    send(sockfd,&train,sizeof(train.length)+train.length, MSG_NOSIGNAL);
//
//    while(1){
//        bzero(train.data,sizeof(train.data));
//        ssize_t sret = read(fd,train.data,sizeof(train.data)); // 读磁盘文件内容
//        train.length = sret;
//        send(sockfd,&train,sizeof(train.length)+train.length, MSG_NOSIGNAL);
//        if(sret == 0){ //把if写在send后面，最后一次会发空火车
//            break;
//        }
//    }
//    close(fd);
//    return 0;
//}
// v4.1 发大文件 用私有协议 发文件长度 mmap
//int transfile(int sockfd){
//    train_t train;
//    char filename[] = "file1";
//    train.length = 5;
//    memcpy(train.data,filename,train.length);
//    send(sockfd,&train,sizeof(train.length)+train.length, MSG_NOSIGNAL);
//
//    int fd = open(filename,O_RDWR);
//    struct stat statbuf;
//    fstat(fd,&statbuf);
//    off_t filesize = statbuf.st_size;//这个数据可以直接发，建议放入小火车
//    train.length = sizeof(filesize);
//    memcpy(train.data,&filesize,train.length);
//    send(sockfd,&train,sizeof(train.length)+train.length, MSG_NOSIGNAL);
//
//    //mmap之前先ftruncate
//    ftruncate(fd,filesize);
//    //建立映射区
//    char *p = (char *)mmap(NULL,filesize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
//    off_t offset = 0; //已经发送的长度
//    while(1){
//        if(offset >= filesize){
//            break;
//        }
//        if(filesize - offset > 1000){
//            train.length = 1000;
//        }
//        else{
//            train.length = filesize - offset;
//        }
//        send(sockfd,&train.length,sizeof(train.length),MSG_NOSIGNAL);
//        send(sockfd,p+offset,train.length,MSG_NOSIGNAL);//p指向映射区 send是从内核态到内核态
//        offset += train.length;
//    }
//    train.length = 0;
//    send(sockfd,&train.length,sizeof(train.length),MSG_NOSIGNAL); // 发一个空的
//    //释放映射区
//    munmap(p,filesize);
//    close(fd);
//    return 0;
//}
// v5.0 发大文件 发文件长度 mmap 发内容不用小火车
int transfile(int sockfd){
    train_t train;
    char filename[] = "file1";
    train.length = 5;
    memcpy(train.data,filename,train.length);
    send(sockfd,&train,sizeof(train.length)+train.length, MSG_NOSIGNAL);

    int fd = open(filename,O_RDWR);
    struct stat statbuf;
    fstat(fd,&statbuf);
    off_t filesize = statbuf.st_size;//这个数据可以直接发，建议放入小火车
    train.length = sizeof(filesize);
    memcpy(train.data,&filesize,train.length);
    send(sockfd,&train,sizeof(train.length)+train.length, MSG_NOSIGNAL);

    ////mmap之前先ftruncate
    //ftruncate(fd,filesize);
    ////建立映射区
    //char *p = (char *)mmap(NULL,filesize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    //send(sockfd,p,filesize,MSG_NOSIGNAL);//一次性发完
    ////释放映射区
    //munmap(p,filesize);
    sleep(10);
    sendfile(sockfd,fd,NULL,filesize);
    close(fd);
    return 0;
}
