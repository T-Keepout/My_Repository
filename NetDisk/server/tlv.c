#include "tlv.h"

tlv_t* tlv_create(uint8_t type, const void* data, uint16_t len) {
    // 检查参数有效性
    if ( len > 0 && data == NULL ) {
        fprintf(stderr, "错误: 非空长度需要有效数据指针\n");
        return NULL;
    }

    // 计算总内存大小（结构体 + 数据区）
    size_t total_size = sizeof(tlv_t) + len;
    tlv_t* tlv = (tlv_t*)malloc(total_size);
    if ( tlv == NULL ) {
        perror("内存分配失败");
        return NULL;
    }

    // 填充TLV头部
    tlv->type = type;
    tlv->len = len;

    // 如果有数据则拷贝
    if ( len > 0 && data != NULL ) {
        memcpy(tlv->value, data, len);
    }

    return tlv;
}

void tlv_free(tlv_t* tlv) {
    if ( tlv ) {
        free(tlv);
    }
}

int tlv_send(int sockfd, tlv_t* tlv) {
    // 发送头部 (type + len)
    uint8_t header[3];
    header[0] = tlv->type;
    *(uint16_t*)(header + 1) = htons(tlv->len); // 网络字节序

    if ( send(sockfd, header, 3, 0) != 3 ) {
        return -1; // 发送头部失败
    }

    // 发送数据体 (如果有)
    if ( tlv->len > 0 && send(sockfd, tlv->value, tlv->len, 0) != tlv->len ) {
        return -1; // 发送数据失败
    }
    return 0;
}

tlv_t* tlv_recv(int sockfd) {
    // 接收头部
    uint8_t header[3];
    if ( recv(sockfd, header, 3, MSG_WAITALL) != 3 ) {
        return NULL; // 接收头部失败
    }

    // 解析头部
    uint8_t type = header[0];
    uint16_t len = ntohs(*(uint16_t*)(header + 1)); // 主机字节序

    // 创建TLV包
    tlv_t* tlv = (tlv_t*)malloc(sizeof(tlv_t) + len);
    if ( !tlv ) return NULL;

    tlv->type = type;
    tlv->len = len;

    // 接收数据体 (如果有)
    if ( len > 0 && recv(sockfd, tlv->value, len, MSG_WAITALL) != len ) {
        free(tlv);
        return NULL;
    }
    return tlv;
}
