#include "session.h"
#include "mysql.h"
#include "tlv.h"

// 路径规范化函数 
int normalize_virtual_path(const char* cwd, const char* input, char* output, size_t outlen) {
    if ( !cwd || !input || !output || outlen == 0 ) return -1;

    // 临时栈用于处理路径组件
    const char* components[256];
    int comp_count = 0;
    char temp[1024];

    // 1. 处理绝对路径或相对路径
    if ( input[0] == '/' ) {
        strncpy(temp, input, sizeof(temp) - 1);
        temp[sizeof(temp) - 1] = 0;
    }
    else {
        snprintf(temp, sizeof(temp), "%s/%s", cwd, input);
    }

    // 2. 清理多余的“/”
    size_t len = strlen(temp);
    char cleaned[1024];
    int j = 0;
    int prev_slash = 0;
    for ( size_t i = 0; i < len && j < (int)sizeof(cleaned) - 1; ++i ) {
        if ( temp[i] == '/' ) {
            if ( !prev_slash ) cleaned[j++] = '/';
            prev_slash = 1;
        }
        else {
            cleaned[j++] = temp[i];
            prev_slash = 0;
        }
    }
    if ( j > 1 && cleaned[j - 1] == '/' ) j--;
    cleaned[j] = '\0';

    // 3. 拆分组件
    char* token;
    char* rest = cleaned;
    char* saveptr = NULL;
    while ( (token = strtok_r(rest, "/", &saveptr)) ) {
        rest = NULL;
        if ( strcmp(token, ".") == 0 ) {
            continue;
        }
        else if ( strcmp(token, "..") == 0 ) {
            if ( comp_count > 0 ) comp_count--;
            // 若已经到根，不再上溯
        }
        else {
            if ( comp_count < 256 )
                components[comp_count++] = token;
            else
                return -1; // 路径过深
        }
    }

    // 4. 拼接规范化路径
    if ( comp_count == 0 ) {
        if ( outlen < 2 ) return -1;
        strncpy(output, "/", outlen - 1);
        output[outlen - 1] = '\0';
        return 0;
    }
    output[0] = '\0';
    for ( int i = 0; i < comp_count; ++i ) {
        if ( strlen(output) + strlen(components[i]) + 2 > outlen ) return -1;
        strncat(output, "/", outlen - strlen(output) - 1);
        strncat(output, components[i], outlen - strlen(output) - 1);
    }
    return 0;
}

// cd命令
int cmd_cd(Session_t* session, const char* path) {
    char abs_path[256];
    //if(path == NULL || strlen(path) == 0){
    //    // 跳转回用户初始目录
    //    strncpy(session->virtual_cwd,session->root_path,sizeof(session->virtual_cwd)-1);
    //    session->virtual_cwd[sizeof(session->virtual_cwd)-1] = '\0';
    //    session->cwd_id = session->root_id;
    //    printf("切换到用户根目录: %s\n", session->virtual_cwd);
    //    return 0;
    // }
    if ( path == NULL || strlen(path) == 0 ) {
        memset(session->virtual_cwd, 0, PATH_MAX);
        session->virtual_cwd[0] = '/';
        session->cwd_id = vf_get_id_by_path_user(session->conn,session->virtual_cwd,session->user_name);
        cmd_ls(session);
    }
    if ( normalize_virtual_path(session->virtual_cwd, path, abs_path, sizeof(abs_path)) != 0 ) {
        //printf("路径规范化失败: %s\n", path);
        tlv_t* empty_tlv = tlv_create((uint8_t)INVALID_DIR, NULL, 0);
        tlv_send(session->netfd, empty_tlv);
        tlv_free(empty_tlv);

        return -1;
    }

    int id = vf_get_id_by_path_user(session->conn, abs_path, session->user_name);
    if ( id > 0 ) {
        session->cwd_id = id;
        strncpy(session->virtual_cwd, abs_path, sizeof(session->virtual_cwd) - 1);
        session->virtual_cwd[sizeof(session->virtual_cwd) - 1] = 0;
        tlv_t* empty_tlv = tlv_create((uint8_t)CMD_SUCCESS, NULL, 0);
        tlv_send(session->netfd, empty_tlv);
        tlv_free(empty_tlv);
        //printf("切换目录成功: %s\n", abs_path);
    }
    else {
        tlv_t* empty_tlv = tlv_create((uint8_t)INVALID_DIR, NULL, 0);
        tlv_send(session->netfd, empty_tlv);
        tlv_free(empty_tlv);
        //printf("目录不存在: %s\n", abs_path);
    }
    return 0;
}

// ls命令
int cmd_ls(Session_t* session) {
    char abs_path[256];
    char file_name_arr[4096];
    int target_id;
    char* user_name = session->user_name;

    // 1.处理路径，如果path为NULL或者空字符串，列出当前目录
        target_id = session->cwd_id;
        strncpy(abs_path, session->virtual_cwd, sizeof(abs_path) - 1);
        abs_path[sizeof(abs_path) - 1] = '\0';
    
    int file_count = 0;     // 统计查询到的目录和文件数量
    VirtualFileInfo_t* infos = vf_list_by_parent_id(session->conn, target_id, session->user_name, &file_count);
    if ( file_count == 0 ) {
        tlv_t* empty_tlv = tlv_create((uint8_t)CMD_SUCCESS, NULL, 0);
        tlv_send(session->netfd, empty_tlv);
        tlv_free(empty_tlv);
        //printf("该目录下无文件或子目录\n");
    }
    else {
        printf("目录%s下文件和目录:\n", abs_path);
        file_name_arr[0] = '\0';
        for ( int i = 0; i < file_count; i++ ) {
            if ( strcmp(infos[i].file_type, "d") == 0 ) {
                size_t curr_len = strlen(file_name_arr);
                snprintf(file_name_arr+curr_len, sizeof(file_name_arr) - curr_len, "#%s", infos[i].file_name);
            }
            else if ( strcmp(infos[i].file_type, "f") == 0 ) {
                size_t curr_len = strlen(file_name_arr);
                snprintf(file_name_arr + curr_len , sizeof(file_name_arr) - curr_len, "$%s", infos[i].file_name);
            }
        }
        file_name_arr[strlen(file_name_arr)] = '\0';
        tlv_t* ls_tlv = tlv_create((uint8_t)CMD_SUCCESS, file_name_arr, strlen(file_name_arr)+1);
        tlv_send(session->netfd, ls_tlv);
        tlv_free(ls_tlv);
    }
    if ( infos )   free(infos);

    return 0;
}

// mkdir命令
int cmd_mkdir(Session_t* session, const char* path) {
    // 1. 合法性检查：不能为空，不能带斜杠，不能为"."或".."
    /*if ( !path || strlen(path) == 0 || strchr(path, '/') || strcmp(path, ".") == 0 || strcmp(path, "..") == 0 ) {
        tlv_t empty_tlv = tlv_create((uint8_t)INVALID_DIR, NULL, 0);
        tlv_send(session->netfd, empty_tlv);
        tlv_free(empty_tlv);
        return -1;
    }*/
    // 2. 检查当前目录下是否已存在同名目录
    if ( vf_exist_in_dir(session->conn, session->cwd_id, path, session->user_name, "dir") ) {
        tlv_t* empty_tlv = tlv_create((uint8_t)MKDIR_FAILED, NULL, 0);
        tlv_send(session->netfd, empty_tlv);
        tlv_free(empty_tlv);
        return -1;
    }
    // 3. 构造虚拟路径
    char new_path[512];
    if ( strcmp(session->virtual_cwd, "/") == 0 )
        snprintf(new_path, sizeof(new_path), "/%s", path);
    else
        snprintf(new_path, sizeof(new_path), "%s/%s", session->virtual_cwd, path);
    // 4. 插入新目录
    VirtualFileInfo_t info = { 0 };
    strncpy(info.file_name, path, sizeof(info.file_name) - 1);
    strncpy(info.user_name, session->user_name, sizeof(info.user_name) - 1);
    info.parent_id = session->cwd_id;
    info.link = 0;
    strncpy(info.file_type, "dir", sizeof(info.file_type) - 1);
    strncpy(info.path, new_path, sizeof(info.path) - 1);

    if ( vf_insert(session->conn, &info) == 0 ) {
        tlv_t* empty_tlv = tlv_create((uint8_t)CMD_SUCCESS, NULL, 0);
        tlv_send(session->netfd, empty_tlv);
        tlv_free(empty_tlv);
        return 0;
    }
    /*else {
        printf("目录创建失败！\n");
        return -1;
    }*/
}

