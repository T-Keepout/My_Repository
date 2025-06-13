#ifndef __MYSQL_H_
#define __MYSQL_H_

#include <my_header.h>

extern MYSQL* conn;

// 虚拟文件表结构体
typedef struct VirtualFileInfo_s{
    int id;
    char file_name[256];
    char user_name[256];
    int parent_id;
    char sha256[256];
    char file_type[16];
    char path[256];
    int link;
} VirtualFileInfo_t;

// 数据库连接
int db_connect(MYSQL** pconn);

// 往表中增加一条记录
int vf_insert(MYSQL *conn,const VirtualFileInfo_t *info);

// 查：根据虚拟路径和用户查 id
int vf_get_id_by_path_user(MYSQL *conn,const char *path, const char *user_name);

// 查：parent_id下所有文件/目录
VirtualFileInfo_t* vf_list_by_parent_id(MYSQL *conn,int parent_id, const char *user_name, int *count);

// 查：根据id查信息
int vf_get_info_by_id(MYSQL *conn,int id, VirtualFileInfo_t *info);

// 删：根据id删除（可递归目录）
int vf_delete_by_id(MYSQL *conn,int id);

// 查：同目录下是否已有同名
int vf_exist_in_dir(MYSQL *conn,int parent_id, const char *file_name, const char *user_name, const char *file_type);

#endif
