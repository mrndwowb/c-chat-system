#include <stdio.h>
#include <string.h>
#include "user.h"

User users[MAX_USERS];
int userCount = 0;

/* 从 users.txt 读取所有用户 */
void load_users() {
    FILE *fp = fopen("users.txt", "r");
    userCount = 0;

    if (fp == NULL) {
        /* 文件不存在是正常情况（第一次运行） */
        return;
    }

    while (fscanf(fp, "%s %s",
                  users[userCount].username,
                  users[userCount].password) == 2) {
        userCount++;
        if (userCount >= MAX_USERS) break;
    }

    fclose(fp);
}

/* 将所有用户写回 users.txt */
void save_users() {
    FILE *fp = fopen("users.txt", "w");
    if (fp == NULL) return;

    for (int i = 0; i < userCount; i++) {
        fprintf(fp, "%s %s\n",
                users[i].username,
                users[i].password);
    }

    fclose(fp);
}

/* 查找用户，返回 index，不存在返回 -1 */
int find_user(const char *name) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, name) == 0) {
            return i;
        }
    }
    return -1;
}

/* 注册新用户 */
void register_user() {
    char name[MAX_NAME_LEN];
    char pwd1[MAX_NAME_LEN];
    char pwd2[MAX_NAME_LEN];

    printf("Enter account name: ");
    scanf("%s", name);

    if (find_user(name) != -1) {
        printf("Account name already exists.\n");
        return;
    }

    printf("Enter password: ");
    scanf("%s", pwd1);

    printf("Confirm password: ");
    scanf("%s", pwd2);

    if (strcmp(pwd1, pwd2) != 0) {
        printf("Warning! Incorrect password.\n");
        return;
    }

    strcpy(users[userCount].username, name);
    strcpy(users[userCount].password, pwd1);
    userCount++;

    save_users();
    printf("Registration successful! You can now login.\n");
}

/* 登录，成功返回 userIdx，失败返回 -1 */
int login_user() {
    char name[MAX_NAME_LEN];
    char pwd[MAX_NAME_LEN];

    /* sample 要求：无用户时单独提示 */
    if (userCount == 0) {
        printf("No user in this system, please register one.\n");
        return -1;
    }

    printf("Please input your account name: ");
    scanf("%s", name);
    printf("and password: ");
    scanf("%s", pwd);

    int idx = find_user(name);
    if (idx == -1) {
        printf("Warning! Account name not found.\n");
        return -1;
    }

    if (strcmp(users[idx].password, pwd) != 0) {
        printf("Warning! Incorrect password.\n");
        return -1;
    }

    printf("Login successful. Welcome %s!\n", users[idx].username);
    return idx;
}
