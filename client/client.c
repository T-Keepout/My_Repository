#include <65func.h>
typedef struct {
    int length;
    char data[1000];
}train_t;
// v1.0 收小文件
//int recvfile(int sockfd){
//    char filename[4096] = {0};
//    recv(sockfd,filename,sizeof(filename),0);
//    int fd = open(filename,O_RDWR|O_CREAT|O_TRUNC,0666);
//    char buf[1000] = {0};
//    ssize_t sret = recv(sockfd,buf,sizeof(buf),0);
//    write(fd,buf,sret);
//    return 0;
//}
// v2.0
//int recvfile(int sockfd){
//    train_t train;
//    char filename[4096] = {0};
//    recv(sockfd,&train.length,sizeof(train.length),0); //先收4B
//    recv(sockfd,train.data,train.length,0);//再根据火车头收内容
//    memcpy(filename,train.data,train.length);
//
//    int fd = open(filename,O_RDWR|O_CREAT|O_TRUNC,0666);
//    recv(sockfd,&train.length,sizeof(train.length),0); //先收4B
//    recv(sockfd,train.data,train.length,0);//再根据火车头收内容
//    write(fd,train.data,train.length);
//    return 0;
//}
// v3.0
//int recvfile(int sockfd){
//    train_t train;
//    char filename[4096] = {0};
//    recv(sockfd,&train.length,sizeof(train.length),MSG_WAITALL); //先收4B
//    recv(sockfd,train.data,train.length,MSG_WAITALL);//再根据火车头收内容
//    memcpy(filename,train.data,train.length);
//
//    int fd = open(filename,O_RDWR|O_CREAT|O_TRUNC,0666);
//    while(1){
//        recv(sockfd,&train.length,sizeof(train.length),MSG_WAITALL); //先收4B
//        if(train.length != 1000){
//            printf("train.length = %d\n", train.length);
//        }
//        if(train.length == 0){//收到了最后一个空火车
//            break;
//        }
//        recv(sockfd,train.data,train.length,MSG_WAITALL);//再根据火车头收内容
//        write(fd,train.data,train.length);
//    }
//    close(fd);
//    return 0;
//}
// v3.1
int recvn(int sockfd, void *buf, int n){
    // void *的指针不能解引用，也不能偏移
    char *p = (char *)buf;
    int cursize = 0;
    while(cursize < n){
        ssize_t sret = recv(sockfd,p+cursize,n-cursize,0);
        cursize += sret;
    }
    return 0;
}
//int recvfile(int sockfd){
//    train_t train;
//    char filename[4096] = {0};
//    recvn(sockfd,&train.length,sizeof(train.length)); //先收4B
//    recvn(sockfd,train.data,train.length);//再根据火车头收内容
//    memcpy(filename,train.data,train.length);
//
//    int fd = open(filename,O_RDWR|O_CREAT|O_TRUNC,0666);
//    while(1){
//        recvn(sockfd,&train.length,sizeof(train.length)); //先收4B
//        if(train.length != 1000){
//            printf("train.length = %d\n", train.length);
//        }
//        if(train.length == 0){//收到了最后一个空火车
//            break;
//        }
//        sleep(1);
//        recvn(sockfd,train.data,train.length);//再根据火车头收内容
//        write(fd,train.data,train.length);
//    }
//    close(fd);
//    return 0;
//}
//v4.0
//int recvfile(int sockfd){
//    train_t train;
//    char filename[4096] = {0};
//    recvn(sockfd,&train.length,sizeof(train.length)); //先收4B
//    recvn(sockfd,train.data,train.length);//再根据火车头收内容
//    memcpy(filename,train.data,train.length);
//
//    off_t filesize;
//    recvn(sockfd,&train.length,sizeof(train.length)); //先收4B
//    recvn(sockfd,train.data,train.length);//再根据火车头收内容
//    memcpy(&filesize,train.data,train.length);
//    printf("filesize = %ld\n", filesize);
//    off_t cursize = 0;//已经收到的长度
//    off_t lastsize = 0; //上次打印的时候的cursize
//    off_t slice = filesize/10000;
//
//    int fd = open(filename,O_RDWR|O_CREAT|O_TRUNC,0666);
//    while(1){
//        recvn(sockfd,&train.length,sizeof(train.length)); //先收4B
//        if(train.length != 1000){
//            printf("train.length = %d\n", train.length);
//        }
//        if(train.length == 0){//收到了最后一个空火车
//            break;
//        }
//        recvn(sockfd,train.data,train.length);//再根据火车头收内容
//        write(fd,train.data,train.length);
//        cursize += train.length; //累加火车头的内容
//        if(cursize - lastsize > slice){
//            printf("%5.2lf%%\r", cursize*100.0/filesize);
//            fflush(stdout); // 手动刷新缓冲区
//            lastsize = cursize;
//        }
//    }
//
//    printf("100.00%%\n");
//    close(fd);
//    return 0;
//}
//v5.0
int recvfile(int sockfd){
    train_t train;
    char filename[4096] = {0};
    recvn(sockfd,&train.length,sizeof(train.length)); //先收4B
    recvn(sockfd,train.data,train.length);//再根据火车头收内容
    memcpy(filename,train.data,train.length);

    off_t filesize;
    recvn(sockfd,&train.length,sizeof(train.length)); //先收4B
    recvn(sockfd,train.data,train.length);//再根据火车头收内容
    memcpy(&filesize,train.data,train.length);
    printf("filesize = %ld\n", filesize);

    sleep(5);
    int fd = open(filename,O_RDWR|O_CREAT|O_TRUNC,0666);
    ftruncate(fd,filesize);
    char *p = (char *)mmap(NULL,filesize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    recvn(sockfd,p,filesize);
    munmap(p,filesize);
    close(fd);
    return 0;
}
int main(int argc, char *argv[]){
    // ./client 192.168.72.128 12345
    ARGS_CHECK(argc,3);
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr.s_addr  = inet_addr(argv[1]);
    int ret = connect(sockfd,(struct sockaddr *)&addr,sizeof(addr));
    ERROR_CHECK(ret,-1,"connect");
    recvfile(sockfd);
    return 0;
}
