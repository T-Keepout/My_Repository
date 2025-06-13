#ifndef __TLV_H_
#define __TLV_H_

#include <my_header.h>

typedef struct tlv_s {
    uint8_t  type;  // 主类别 (1字节)
    uint16_t len;    // 数据长度 (2字节)
    uint8_t  value[];   // 柔性数组
} tlv_t;

enum {
    //  命令类型:COMMAND,0*
    CMD_SHORT_CD,   // 短的cd,不带数据
    CMD_SHORT_LS,   // 短的cd，不带数据
    CMD_LONG_CD,    // 长的cd，携带路径
    CMD_MKDIR,      // 列出文件
    CMD_REMOVE,         // 删除文件
    CMD_UPLOAD,     // 上传
    CMD_DOWNLOAD,   // 下载
    // 相应类型:RESPONSE暂时保留,3*
    SUCCESS_REGIS = 48, // 注册成功
    SUCCESS_LOGIN,      // 登录成功
    INVALID_DIR,        // 非法目录
    CMD_SUCCESS,        // 命令成功

    MKDIR_FAILED,  //  创建目录成功
    REMOVE_DIR_FAILED,  //  删除目录失败
                                                      
    UPLOAD_FILE_FAILED, // 上传文件失败 
    UPLOAD_FILE_SUCCESS,// 上传文件成功 
    DOWNLOAD_FILE_FAILED,// 下载文件失败 
    DOWNLOAD_FILE_SUCCESS,// 下载文件成功 
};

tlv_t* tlv_create(uint8_t type, const void* data, uint16_t len);

// TLV释放函数
// 参数: tlv - 要释放的TLV指针
void tlv_free(tlv_t* tlv);

// 发送TLV包
int tlv_send(int sockfd, tlv_t* tlv);

// 接收TLV包
tlv_t* tlv_recv(int sockfd);

#endif

