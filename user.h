#ifndef USER_H
#define USER_H

#define MAX_USERS 100
#define MAX_NAME_LEN 30

typedef struct {
    char username[MAX_NAME_LEN];
    char password[MAX_NAME_LEN];
} User;

/* 全局用户数据（由 user.c 管理） */
extern User users[MAX_USERS];
extern int userCount;

/* A 模块提供的接口 */
void load_users();
void save_users();
int find_user(const char *name);
void register_user();
int login_user();

#endif
