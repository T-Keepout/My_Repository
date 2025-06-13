#ifndef __SESSION_H_
#define __SESSION_H_

#include <my_header.h>

typedef struct Session_s {
    int netfd;
    char user_name[256];
    char virtual_cwd[PATH_MAX]; // 用户当前目录
    int cwd_id;                 // 当前目录id
    MYSQL* conn;
}Session_t;

// 路径规范化，将cwd和用户输入的路径input拼接成为规范化的虚拟路径，将结果存到output
int normalize_virtual_path(const char* cwd, const char* input, char* output, size_t outlen);

// 目录操作命令
// 1.cd命令
int cmd_cd(Session_t* session,const char* path);

// 2.ls命令
int cmd_ls(Session_t* session);

// 3.mkdir命令
int cmd_mkdir(Session_t* session, const char* path);

// 4.remove命令
int cmd_remove(Session_t* session, const char* path);

// 5.upload命令，文件上传接口
int cmd_upload(Session_t* session, const char* path);

// 6.download命令，文件下载接口
int cmd_download(Session_t* session, const char* path);

// 7.命令分发函数
int handle_command(Session_t* session, const char* cmd, uint8_t cmdtype);

#endif

