#include <66func.h>
// v 1.0
//int recvfile(int sockfd){
//    char filename[4096] = {0};
//    recv(sockfd,filename,sizeof(filename), 0);
//    int fd = open(filename,O_RDWR|O_CREAT|O_TRUNC,0666);
//    char buf[4096] = {0};
//    ssize_t sret = recv(sockfd,buf,sizeof(buf),0);
//    write(fd,buf,sret);
//    return 0;
//}
typedef struct train_s {
    int length;
    char data[1000];
} train_t;//客户端和服务端协议设计要一致
// v 2.0
//int recvfile(int sockfd){
//    train_t train;
//    // 先收4字节火车头 再收火车头内容的长度，再拷贝到filename
//    recv(sockfd,&train.length,sizeof(train.length),0);
//    recv(sockfd,train.data,train.length,0);
//    char filename[4096] = {0};
//    memcpy(filename,train.data,train.length);
//
//    int fd = open(filename,O_RDWR|O_CREAT|O_TRUNC,0666);
//    recv(sockfd,&train.length,sizeof(train.length),0);
//    recv(sockfd,train.data,train.length,0);
//    write(fd,train.data,train.length);
//    close(fd);
//    return 0;
//}
// v 3.0
//int recvfile(int sockfd){
//    train_t train;
//    // 先收4字节火车头 再收火车头内容的长度，再拷贝到filename
//    recv(sockfd,&train.length,sizeof(train.length),MSG_WAITALL);
//    recv(sockfd,train.data,train.length,MSG_WAITALL);
//    char filename[4096] = {0};
//    memcpy(filename,train.data,train.length);
//
//    int fd = open(filename,O_RDWR|O_CREAT|O_TRUNC,0666);
//    while(1){
//        bzero(train.data,sizeof(train.data));
//        recv(sockfd,&train.length,sizeof(train.length),MSG_WAITALL);
//        if(train.length != 1000){
//            printf("train.length = %d\n", train.length);
//        }
//        if(train.length == 0){
//            break;
//        }
//        recv(sockfd,train.data,train.length,MSG_WAITALL);
//        write(fd,train.data,train.length);
//    }
//    close(fd);
//    return 0;
//}
// v 3.1
int recvn(int sockfd, void *buf, ssize_t length){
    char *p = (char *)buf;//char* 是因为偏移以1个字节为单位
    ssize_t cursize = 0;
    while(cursize < length){
        ssize_t sret = recv(sockfd,p+cursize,length-cursize,0);
        cursize += sret;
    }
    return 0;
}
//int recvfile(int sockfd){
//    train_t train;
//    // 先收4字节火车头 再收火车头内容的长度，再拷贝到filename
//    recvn(sockfd,&train.length,sizeof(train.length));
//    recvn(sockfd,train.data,train.length);
//    char filename[4096] = {0};
//    memcpy(filename,train.data,train.length);
//    int fd = open(filename,O_RDWR|O_CREAT|O_TRUNC,0666);
//    while(1){
//        bzero(train.data,sizeof(train.data));
//        sleep(1);
//        recvn(sockfd,&train.length,sizeof(train.length));
//        if(train.length != 1000){
//            printf("train.length = %d\n", train.length);
//        }
//        if(train.length == 0){
//            break;
//        }
//        recvn(sockfd,train.data,train.length);
//        write(fd,train.data,train.length);
//    }
//    close(fd);
//    return 0;
//}
//v4.0
int recvfile(int sockfd){
    train_t train;
    // 先收4字节火车头 再收火车头内容的长度，再拷贝到filename
    recvn(sockfd,&train.length,sizeof(train.length));
    recvn(sockfd,train.data,train.length);
    char filename[4096] = {0};
    memcpy(filename,train.data,train.length);
    int fd = open(filename,O_RDWR|O_CREAT|O_TRUNC,0666);

    off_t filesize;
    off_t cursize = 0;
    off_t lastsize = 0;
    recvn(sockfd,&train.length,sizeof(train.length));
    recvn(sockfd,train.data,train.length);
    memcpy(&filesize,train.data,train.length);
    printf("filesize = %ld\n", filesize);
    off_t slice = filesize/10000;

    sleep(10);
    while(1){
        bzero(train.data,sizeof(train.data));
        recvn(sockfd,&train.length,sizeof(train.length));
        if(train.length != 1000){
            printf("train.length = %d\n", train.length);
        }
        if(train.length == 0){
            break;
        }
        cursize += train.length;
        if(cursize - lastsize > slice){
            printf("%5.2lf%%\r",100.0*cursize/filesize);
            fflush(stdout);
            lastsize = cursize;
        }
        recvn(sockfd,train.data,train.length);
        write(fd,train.data,train.length);
    }
    printf("100.00%%\n");
    close(fd);
    return 0;
}
//v 5.0
//int recvfile(int sockfd){
//    train_t train;
//    // 先收4字节火车头 再收火车头内容的长度，再拷贝到filename
//    recvn(sockfd,&train.length,sizeof(train.length));
//    recvn(sockfd,train.data,train.length);
//    char filename[4096] = {0};
//    memcpy(filename,train.data,train.length);
//    int fd = open(filename,O_RDWR|O_CREAT|O_TRUNC,0666);
//
//    off_t filesize;
//    recvn(sockfd,&train.length,sizeof(train.length));
//    recvn(sockfd,train.data,train.length);
//    memcpy(&filesize,train.data,train.length);
//    printf("filesize = %ld\n", filesize);
//
//    ftruncate(fd,filesize);
//    char * p = (char *)mmap(NULL,filesize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
//    recvn(sockfd,p,filesize);
//    munmap(p,filesize);
//    close(fd);
//    return 0;
//}
int main(int argc, char *argv[])
{
    // ./01_client_tcp_chat 192.168.72.128 12345
    ARGS_CHECK(argc,3);
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    int ret = connect(sockfd,(struct sockaddr *)&addr,sizeof(addr));
    ERROR_CHECK(ret,-1,"connect");
    recvfile(sockfd);
    close(sockfd);
    return 0;
}

