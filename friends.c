#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "friends.h"

/* 全局好友表 */
FriendList friendLists[MAX_USERS];

/* ================= 文件读写 ================= */

void load_friends() {
    FILE *fp = fopen("friends.txt", "r");
    if (fp == NULL) {
        /* 第一次运行，初始化为空 */
        for (int i = 0; i < userCount; i++) {
            friendLists[i].friendCount = 0;
            friendLists[i].pendingCount = 0;
        }
        return;
    }

    for (int i = 0; i < userCount; i++) {
        /* 读取 friendCount */
        fscanf(fp, "%d", &friendLists[i].friendCount);
        for (int j = 0; j < friendLists[i].friendCount; j++) {
            fscanf(fp, "%s", friendLists[i].friends[j]);
        }

        /* 读取 pendingCount */
        fscanf(fp, "%d", &friendLists[i].pendingCount);
        for (int j = 0; j < friendLists[i].pendingCount; j++) {
            fscanf(fp, "%s", friendLists[i].pendings[j]);
        }
    }

    fclose(fp);
}

void save_friends() {
    FILE *fp = fopen("friends.txt", "w");
    if (fp == NULL) return;

    for (int i = 0; i < userCount; i++) {
        fprintf(fp, "%d\n", friendLists[i].friendCount);
        for (int j = 0; j < friendLists[i].friendCount; j++) {
            fprintf(fp, "%s\n", friendLists[i].friends[j]);
        }

        fprintf(fp, "%d\n", friendLists[i].pendingCount);
        for (int j = 0; j < friendLists[i].pendingCount; j++) {
            fprintf(fp, "%s\n", friendLists[i].pendings[j]);
        }
    }

    fclose(fp);
}

/* ================= 工具函数 ================= */

int is_friend(int userIdx, const char *name) {
    for (int i = 0; i < friendLists[userIdx].friendCount; i++) {
        if (strcmp(friendLists[userIdx].friends[i], name) == 0)
            return 1;
    }
    return 0;
}

/* ================= 功能实现 ================= */

void show_friends(int userIdx) {
    if (friendLists[userIdx].friendCount == 0) {
        printf("You have no friends.\n");
        return;
    }

    char temp[MAX_FRIENDS][USERNAME_LEN];
    int n = friendLists[userIdx].friendCount;

    /* copy friends */
    for (int i = 0; i < n; i++) {
        strcpy(temp[i], friendLists[userIdx].friends[i]);
    }

    /* sort by name (alphabetical order) */
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (strcmp(temp[i], temp[j]) > 0) {
                char t[USERNAME_LEN];
                strcpy(t, temp[i]);
                strcpy(temp[i], temp[j]);
                strcpy(temp[j], t);
            }
        }
    }

    printf("Your friends:\n");
    for (int i = 0; i < n; i++) {
        printf("%d. %s\n", i + 1, temp[i]);
    }
}

/* ---------- Add friends ---------- */

void add_friends(int userIdx) {
    char line[256];
    printf("Enter usernames to add (in one line separated by space): ");
    fgets(line, sizeof(line), stdin);

    char *token = strtok(line, " \n");
    while (token) {
        /* 不能加自己 */
        if (strcmp(token, users[userIdx].username) == 0) {
            printf("You cannot send a friend request to yourself.\n");
            token = strtok(NULL, " \n");
            continue;
        }

        int targetIdx = find_user(token);
        if (targetIdx == -1) {
            printf("Account %s does not exist.\n", token);
            token = strtok(NULL, " \n");
            continue;
        }

        /* 已经是好友 */
        if (is_friend(userIdx, token)) {
            token = strtok(NULL, " \n");
            continue;
        }

        /* If the target user has already sent a request to current user */
        int reversePending = 0;
        for (int i = 0; i < friendLists[userIdx].pendingCount; i++) {
            if (strcmp(friendLists[userIdx].pendings[i], token) == 0) {
                reversePending = 1;
                break;
            }
        }
        if (reversePending) {
            printf("%s has sent friend request to you.\n", token);
            token = strtok(NULL, " \n");
            continue;
        }

        /* 已在对方 pending 中 */
        int exists = 0;
        for (int i = 0; i < friendLists[targetIdx].pendingCount; i++) {
            if (strcmp(friendLists[targetIdx].pendings[i],
                       users[userIdx].username) == 0) {
                exists = 1;
                break;
            }
        }

        if (exists) {
            printf("Friend request to %s is already pending.\n", token);

        } else {
            strcpy(friendLists[targetIdx]
                       .pendings[friendLists[targetIdx].pendingCount++],
                   users[userIdx].username);
            printf("Friend request sent to %s.\n", token);
        }

        token = strtok(NULL, " \n");
    }

    save_friends();
}

/* ---------- Accept friends ---------- */
void accept_friends(int userIdx) {
    char *token;

    if (friendLists[userIdx].pendingCount == 0) {
        printf("No pending friend requests for %s.\n",
               users[userIdx].username);
        return;
    }

    printf("Pending friend requests for %s:\n",
           users[userIdx].username);

    for (int i = 0; i < friendLists[userIdx].pendingCount; i++) {
        printf("%d. %s\n", i + 1, friendLists[userIdx].pendings[i]);
    }

    printf("%d. All\n", friendLists[userIdx].pendingCount + 1);
    printf("%d. Back\n", friendLists[userIdx].pendingCount + 2);
    printf("Enter indices (space separated), press Enter to finish: ");

    char line[256];
    fgets(line, sizeof(line), stdin);

    int backIdx = friendLists[userIdx].pendingCount + 2;
    int allIdx  = friendLists[userIdx].pendingCount + 1;

    /* 收集选择 */
    int indices[100];
    int cnt = 0;
    int acceptAll = 0;

    token = strtok(line, " \n");
    while (token) {
        int choice = atoi(token);

        if (choice == backIdx) {
            return;
        }
        if (choice == allIdx) {
            acceptAll = 1;
        } else {
            indices[cnt++] = choice - 1;
        }
        token = strtok(NULL, " \n");
    }

    /* Accept all */
    if (acceptAll) {
        for (int i = 0; i < friendLists[userIdx].pendingCount; i++) {
            int fIdx = find_user(friendLists[userIdx].pendings[i]);

            strcpy(friendLists[userIdx]
                       .friends[friendLists[userIdx].friendCount++],
                   users[fIdx].username);

            strcpy(friendLists[fIdx]
                       .friends[friendLists[fIdx].friendCount++],
                   users[userIdx].username);

            printf("Friend requests updated for %s.\n",
                   users[fIdx].username);
        }

        friendLists[userIdx].pendingCount = 0;
        save_friends();
        return;
    }

    /* 从大到小排序，避免数组移位 */
    for (int i = 0; i < cnt - 1; i++) {
        for (int j = i + 1; j < cnt; j++) {
            if (indices[i] < indices[j]) {
                int t = indices[i];
                indices[i] = indices[j];
                indices[j] = t;
            }
        }
    }

    /* Accept selected */
    for (int k = 0; k < cnt; k++) {
        int idx = indices[k];
        if (idx >= 0 && idx < friendLists[userIdx].pendingCount) {
            int fIdx = find_user(friendLists[userIdx].pendings[idx]);

            strcpy(friendLists[userIdx]
                       .friends[friendLists[userIdx].friendCount++],
                   users[fIdx].username);

            strcpy(friendLists[fIdx]
                       .friends[friendLists[fIdx].friendCount++],
                   users[userIdx].username);

            printf("Friend requests updated for %s.\n",
                   users[fIdx].username);

            for (int j = idx; j < friendLists[userIdx].pendingCount - 1; j++) {
                strcpy(friendLists[userIdx].pendings[j],
                       friendLists[userIdx].pendings[j + 1]);
            }
            friendLists[userIdx].pendingCount--;
        }
    }

    save_friends();
}

/* ---------- Delete friends ---------- */
void delete_friends(int userIdx) {
    if (friendLists[userIdx].friendCount == 0) {
        printf("You have no friends.\n");
        return;
    }

    printf("Your friends:\n");
    for (int i = 0; i < friendLists[userIdx].friendCount; i++) {
        printf("%d. %s\n", i + 1, friendLists[userIdx].friends[i]);
    }
    printf("%d. All\n", friendLists[userIdx].friendCount + 1);
    printf("%d. Back\n", friendLists[userIdx].friendCount + 2);
    printf("Enter friend numbers (separated by space), press Enter to finish: ");

    char line[256];
    fgets(line, sizeof(line), stdin);

    /* Back */
    if (atoi(line) == friendLists[userIdx].friendCount + 2) {
        return;
    }

    int deleteAll = 0;
    char *token = strtok(line, " \n");
    while (token) {
        if (atoi(token) == friendLists[userIdx].friendCount + 1) {
            deleteAll = 1;
            break;
        }
        token = strtok(NULL, " \n");
    }

    /* Delete all friends */
    if (deleteAll) {
        printf("Deleting all...\n");

        for (int i = 0; i < friendLists[userIdx].friendCount; i++) {
            int fIdx = find_user(friendLists[userIdx].friends[i]);

            /* 从对方好友列表中删除自己 */
            for (int j = 0; j < friendLists[fIdx].friendCount; j++) {
                if (strcmp(friendLists[fIdx].friends[j],
                        users[userIdx].username) == 0) {
                    for (int k = j; k < friendLists[fIdx].friendCount - 1; k++) {
                        strcpy(friendLists[fIdx].friends[k],
                            friendLists[fIdx].friends[k + 1]);
                    }
                    friendLists[fIdx].friendCount--;
                    break;
                }
            }
        }

        friendLists[userIdx].friendCount = 0;
        printf("Friend list updated.\n");
        save_friends();
        return;
    }


    /* Delete selected friends */
    int indices[100];
    int cnt = 0;

    /* 收集索引 */
    token = strtok(line, " \n");
    while (token) {
        int choice = atoi(token);
        if (choice >= 1 && choice <= friendLists[userIdx].friendCount) {
            indices[cnt++] = choice - 1;
        }
        token = strtok(NULL, " \n");
    }

    /* 从大到小排序，避免数组移位 */
    for (int i = 0; i < cnt - 1; i++) {
        for (int j = i + 1; j < cnt; j++) {
            if (indices[i] < indices[j]) {
                int t = indices[i];
                indices[i] = indices[j];
                indices[j] = t;
            }
        }
    }

    /* 依次删除 */
    for (int k = 0; k < cnt; k++) {
        int idx = indices[k];
        if (idx >= 0 && idx < friendLists[userIdx].friendCount) {
            int fIdx = find_user(friendLists[userIdx].friends[idx]);

            printf("Deleting %s...\n", friendLists[userIdx].friends[idx]);

            /* 从对方好友列表中删除自己 */
            for (int j = 0; j < friendLists[fIdx].friendCount; j++) {
                if (strcmp(friendLists[fIdx].friends[j],
                        users[userIdx].username) == 0) {
                    for (int k2 = j; k2 < friendLists[fIdx].friendCount - 1; k2++) {
                        strcpy(friendLists[fIdx].friends[k2],
                            friendLists[fIdx].friends[k2 + 1]);
                    }
                    friendLists[fIdx].friendCount--;
                    break;
                }
            }

            /* 从自己好友列表中删除对方 */
            for (int j = idx; j < friendLists[userIdx].friendCount - 1; j++) {
                strcpy(friendLists[userIdx].friends[j],
                    friendLists[userIdx].friends[j + 1]);
            }
            friendLists[userIdx].friendCount--;
        }
    }


    printf("Friend list updated.\n");
    save_friends();
}

/* ---------- Manage friends 菜单 ---------- */

void manage_friends(int userIdx) {
    int c;
    do {
        printf("\n==================== Manage Friends ====================\n");
        printf("1. Add friends\n");
        printf("2. Accept friends\n");
        printf("3. Delete friends\n");
        printf("4. Show current friends\n");
        printf("5. Back\n");
        printf("=======================================================\n");
        printf("Choose an option (1-5): ");
        scanf("%d", &c);
        getchar();

        if (c == 1) add_friends(userIdx);
        else if (c == 2) accept_friends(userIdx);
        else if (c == 3) delete_friends(userIdx);
        else if (c == 4) show_friends(userIdx);

    } while (c != 5);
}
