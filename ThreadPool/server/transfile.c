#include "threadPool.h"
// v 1.0 传输一个小文件
//int transfile(int sockfd){
//    char filename[] = "file1";
//    send(sockfd,filename,5,0);
//    int fd = open(filename,O_RDWR);
//    char buf[1000] = {0};
//    ssize_t sret = read(fd,buf,sizeof(buf));
//    // sret是读取内容的长度
//    send(sockfd,buf,sret,0);//文本和二进制通用
//    return 0;
//}
typedef struct train_s {
    int length;
    char data[1000]; //char数组在此处不是字符串的意思
    // 1000是指长度上限
} train_t;
// v 2.0 传输一个小文件
//int transfile(int sockfd){
//    train_t train;
//    char filename[] = "file1";
//    train.length = 5;//火车头
//    memcpy(train.data,filename,train.length);
//    send(sockfd,&train,sizeof(train.length)+train.length,0);//不能写sizeof(train)
//    int fd = open(filename,O_RDWR);
//    ssize_t sret = read(fd,train.data,sizeof(train.data));
//    // sret是读取内容的长度
//    train.length = sret;
//    send(sockfd,&train,sizeof(train.length)+train.length,0);//不能写sizeof(train)
//    return 0;
//}
// v 3.0 传输一个大文件
//int transfile(int sockfd){
//    train_t train;
//    char filename[] = "file1";
//    train.length = 5;//火车头
//    memcpy(train.data,filename,train.length);
//    send(sockfd,&train,sizeof(train.length)+train.length,MSG_NOSIGNAL);//不能写sizeof(train)
//
//    int fd = open(filename,O_RDWR);
//    while(1){
//        bzero(train.data,sizeof(train.data));
//        ssize_t sret = read(fd,train.data,sizeof(train.data));
//        train.length = sret;
//        send(sockfd,&train,sizeof(train.length)+train.length,MSG_NOSIGNAL);//不能写sizeof(train)
//        if(sret == 0){
//            break;
//        }
//    }
//    return 0;
//}
// v 4.0 传输一个大文件 带长度
//int transfile(int sockfd){
//    train_t train;
//    char filename[] = "file1";
//    train.length = 5;//火车头
//    memcpy(train.data,filename,train.length);
//    send(sockfd,&train,sizeof(train.length)+train.length,MSG_NOSIGNAL);//不能写sizeof(train)
//
//    int fd = open(filename,O_RDWR);
//    struct stat statbuf;
//    fstat(fd,&statbuf);
//    printf("filesize = %ld\n", statbuf.st_size);
//    off_t filesize = statbuf.st_size;
//    train.length = sizeof(filesize);
//    memcpy(train.data,&filesize,sizeof(off_t));
//    send(sockfd,&train,sizeof(train.length)+train.length,MSG_NOSIGNAL);//不能写sizeof(train)
//
//    while(1){
//        bzero(train.data,sizeof(train.data));
//        ssize_t sret = read(fd,train.data,sizeof(train.data));
//        train.length = sret;
//        send(sockfd,&train,sizeof(train.length)+train.length,MSG_NOSIGNAL);//不能写sizeof(train)
//        if(sret == 0){
//            break;
//        }
//    }
//    return 0;
//}
// v 4.1 传输一个大文件 带长度 mmap
int transfile(int sockfd){
    train_t train;
    char filename[] = "file1";
    train.length = 5;//火车头
    memcpy(train.data,filename,train.length);
    send(sockfd,&train,sizeof(train.length)+train.length,MSG_NOSIGNAL);//不能写sizeof(train)

    int fd = open(filename,O_RDWR);
    struct stat statbuf;
    fstat(fd,&statbuf);
    printf("filesize = %ld\n", statbuf.st_size);
    off_t filesize = statbuf.st_size;
    train.length = sizeof(filesize);
    memcpy(train.data,&filesize,sizeof(off_t));
    send(sockfd,&train,sizeof(train.length)+train.length,MSG_NOSIGNAL);//不能写sizeof(train)

    // 先执行ftruncate
    ftruncate(fd,filesize);
    // 建立映射区
    char * p = (char *)mmap(NULL,filesize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    off_t offset = 0;
    while(1){
        if(offset >= filesize){
            break; //发完了
        }
        if(filesize - offset > 1000){
            train.length = 1000; //剩余内容超过1000 本次只发1000
        }
        else{
            train.length = filesize - offset; //剩余内容不足1000 本次发剩下的
        }
        send(sockfd,&train.length,sizeof(train.length), MSG_NOSIGNAL);
        send(sockfd, p+offset, train.length, MSG_NOSIGNAL); //从内核态到内核态
        offset += train.length;
    }
    // 最后发一个空火车
    train.length = 0;
    send(sockfd,&train.length,sizeof(train.length),MSG_NOSIGNAL);
    // 释放映射区
    munmap(p,filesize);
    return 0;
}
// v 5.0 传输一个大文件 带长度 mmap 文件内容不用小火车
//int transfile(int sockfd){
//    train_t train;
//    char filename[] = "file1";
//    train.length = 5;//火车头
//    memcpy(train.data,filename,train.length);
//    send(sockfd,&train,sizeof(train.length)+train.length,MSG_NOSIGNAL);//不能写sizeof(train)
//
//    int fd = open(filename,O_RDWR);
//    struct stat statbuf;
//    fstat(fd,&statbuf);
//    printf("filesize = %ld\n", statbuf.st_size);
//    off_t filesize = statbuf.st_size;
//    train.length = sizeof(filesize);
//    memcpy(train.data,&filesize,sizeof(off_t));
//    send(sockfd,&train,sizeof(train.length)+train.length,MSG_NOSIGNAL);//不能写sizeof(train)
//
//   // // 先执行ftruncate
//   // ftruncate(fd,filesize);
//   // // 建立映射区
//   // char * p = (char *)mmap(NULL,filesize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
//   // send(sockfd,p,filesize,MSG_NOSIGNAL);
//   // // 释放映射区
//   // munmap(p,filesize);
//    sendfile(sockfd,fd,NULL,filesize);
//    return 0;
//}
