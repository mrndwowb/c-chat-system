#ifndef FRIENDS_H
#define FRIENDS_H

#include "user.h"

#define MAX_FRIENDS 100
#define USERNAME_LEN 20
#define USERNAME_LEN 20
/* 好友数据结构（按 userIdx 对应） */
typedef struct {
    char friends[MAX_FRIENDS][MAX_NAME_LEN];
    int friendCount;

    char pendings[MAX_FRIENDS][MAX_NAME_LEN];
    int pendingCount;
} FriendList;

/* 全局好友表（下标与 users[] 对齐） */
extern FriendList friendLists[MAX_USERS];

/* B 模块接口 */
void load_friends();
void save_friends();

void manage_friends(int userIdx);
void add_friends(int userIdx);
void accept_friends(int userIdx);
void delete_friends(int userIdx);
void show_friends(int userIdx);



int is_friend(int userIdx, const char *name);

#endif
