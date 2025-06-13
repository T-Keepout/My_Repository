#include "mysql.h"

// 数据库连接函数
int db_connect(MYSQL** pconn){
    *pconn = mysql_init(NULL);

    const char* host = "localhost";
    const char* usr = "root";
    const char* passwd = "1234";
    const char* db = "NetDisk";

    MYSQL* conn = mysql_real_connect(*pconn,host,usr, passwd, db, 0, NULL, 0);
    PSQL_ERROR_CHECK(conn,NULL,*pconn,"mysql_real_connect");

    return 0;
}

// 用户名查重
int ui_exit_user_name

// 往vf表中插入一条记录
int vf_insert(MYSQL* conn,const VirtualFileInfo_t *info) {
    char query[4096];
    snprintf(query, sizeof(query),
        "INSERT INTO virtual_file (file_name, user_name, parent_id, sha256, file_type, path, link) "
        "VALUES ('%s', '%s', %d, '%s', '%s', '%s', %d)",
        info->file_name, info->user_name, info->parent_id, info->sha256,
        info->file_type, info->path, info->link);
    int ret = mysql_query(conn, query);
    SQL_ERROR_CHECK(ret, 0, conn, "insert virtual_file");
    return 0;
}

// 根据虚拟路径和用户名获取文件id
int vf_get_id_by_path_user(MYSQL* conn,const char *path, const char *user_name){
    char query[512];
    snprintf(query, sizeof(query),
        "SELECT id FROM virtual_file WHERE path='%s' AND user_name='%s'", path, user_name);
    int ret = mysql_query(conn, query);
    SQL_ERROR_CHECK(ret, 0, conn, "select id by path");
    MYSQL_RES *result = mysql_store_result(conn);
    if (!result) return -1;
    MYSQL_ROW row = mysql_fetch_row(result);
    int id = -1;
    if (row) id = atoi(row[0]);
    mysql_free_result(result);
    return id;
}

// 获取parent_id下所有文件/目录
VirtualFileInfo_t* vf_list_by_parent_id(MYSQL* conn,int parent_id, const char *user_name, int *count) {
    char query[512];
    snprintf(query, sizeof(query),
        "SELECT id, file_name, user_name, parent_id, sha256, file_type, path, link FROM virtual_file "
        "WHERE parent_id=%d AND user_name='%s' order by file_name", parent_id, user_name);
    int ret = mysql_query(conn, query);
    // SQL_ERROR_CHECK(ret, 0, conn, "list by parent");
    if(ret !=0 ){
        fprintf(stderr,"Error:%s\n",mysql_error(conn));
        return NULL;
    }
    MYSQL_RES *result = mysql_store_result(conn);
    if (!result) { *count = 0; return NULL; }
    int rows = mysql_num_rows(result);
    *count = rows;
    if (rows == 0) { mysql_free_result(result); return NULL; }
    VirtualFileInfo_t *infos = (VirtualFileInfo_t*)malloc(rows * sizeof(VirtualFileInfo_t));
    MYSQL_ROW row;
    int idx = 0;
    while ((row = mysql_fetch_row(result))) {
        infos[idx].id = atoi(row[0]);
        strncpy(infos[idx].file_name, row[1], sizeof(infos[idx].file_name) - 1);
        strncpy(infos[idx].user_name, row[2], sizeof(infos[idx].user_name) - 1);
        infos[idx].parent_id = atoi(row[3]);
        strncpy(infos[idx].sha256, row[4], sizeof(infos[idx].sha256) - 1);
        strncpy(infos[idx].file_type, row[5], sizeof(infos[idx].file_type) - 1);
        strncpy(infos[idx].path, row[6], sizeof(infos[idx].path) - 1);
        infos[idx].link = atoi(row[7]);
        idx++;
    }
    mysql_free_result(result);
    return infos;
}

// 根据id查信息
int vf_get_info_by_id(MYSQL *conn, int id, VirtualFileInfo_t *info) {
    char query[256];
    snprintf(query, sizeof(query),
        "SELECT id, file_name, user_name, parent_id, sha256, file_type, path, link FROM virtual_file WHERE id=%d", id);
    int ret = mysql_query(conn, query);
    SQL_ERROR_CHECK(ret, 0, conn, "get info by id");
    MYSQL_RES *result = mysql_store_result(conn);
    if (!result) return -1;
    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) { mysql_free_result(result); return -1; }
    info->id = atoi(row[0]);
    strncpy(info->file_name, row[1], sizeof(info->file_name) - 1);
    strncpy(info->user_name, row[2], sizeof(info->user_name) - 1);
    info->parent_id = atoi(row[3]);
    strncpy(info->sha256, row[4], sizeof(info->sha256) - 1);
    strncpy(info->file_type, row[5], sizeof(info->file_type) - 1);
    strncpy(info->path, row[6], sizeof(info->path) - 1);
    info->link = atoi(row[7]);
    mysql_free_result(result);
    return 0;
}

// 根据id删除记录
int vf_delete_by_id(MYSQL *conn, int id) {
    // 先查是否目录，若是目录递归删除其所有子节点
    VirtualFileInfo_t info;
    if (vf_get_info_by_id(conn, id, &info) != 0) return -1;
    if (strcmp(info.file_type, "dir") == 0) {
        // 递归删除所有子节点
        int child_count = 0;
        VirtualFileInfo_t* children = vf_list_by_parent_id(conn, id, info.user_name, &child_count);
        for (int i = 0; i < child_count; ++i) {
            vf_delete_by_id(conn, children[i].id);
        }
        if (children) free(children);
    }
    // 删除自身
    char query[128];
    snprintf(query, sizeof(query), "DELETE FROM virtual_file WHERE id=%d", id);
    int ret = mysql_query(conn, query);
    SQL_ERROR_CHECK(ret, 0, conn, "delete by id");
    return 0;
}

// 判断文件或目录在当前目录下是否存在
int vf_exist_in_dir(MYSQL *conn, int parent_id, const char *file_name, const char *user_name, const char *file_type) {
    char query[512];
    snprintf(query, sizeof(query),
        "SELECT id FROM virtual_file WHERE parent_id=%d AND file_name='%s' AND user_name='%s' AND file_type='%s'",
        parent_id, file_name, user_name, file_type);
    int ret = mysql_query(conn, query);
    SQL_ERROR_CHECK(ret, 0, conn, "exist in dir");
    MYSQL_RES *result = mysql_store_result(conn);
    int exist = mysql_num_rows(result) > 0;
    mysql_free_result(result);
    return exist;
}
