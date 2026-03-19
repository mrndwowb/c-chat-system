#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "message.h"
#include "friends.h"
/* ===== function prototype ===== */
void convert_date(const char *src, char *dst);
void convert_date(const char *src, char *dst) {
    int d, m, y;
    sscanf(src, "%d/%d/%d", &d, &m, &y);
    sprintf(dst, "%04d-%02d-%02d", y, m, d);
}

/* ================= 工具函数 ================= */

void get_time_str(char *buf) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
            t->tm_year + 1900,
            t->tm_mon + 1,
            t->tm_mday,
            t->tm_hour,
            t->tm_min,
            t->tm_sec);
}

/* ================= 主菜单 ================= */

void manage_messages(int userIdx) {
    int c;
    do {
        printf("\n==================== Manage Messages ====================\n");
        printf("1. Send a message\n");
        printf("2. Read messages\n");
        printf("3. Delete messages\n");
        printf("4. Back\n");
        printf("=========================================================\n");
        printf("Choose an option (1-4): ");
        scanf("%d", &c);
        getchar();

        if (c == 1) send_message(userIdx);
        else if (c == 2) read_messages(userIdx);
        else if (c == 3) delete_messages(userIdx);

    } while (c != 4);
}

/* ================= Send Message ================= */

void send_message(int userIdx) {
    char content[256], timeStr[32];
    printf("Enter message (max 255 chars), press Enter to finish: ");
    fgets(content, sizeof(content), stdin);
    content[strcspn(content, "\n")] = '\0';

    if (strlen(content) == 0) return;

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

    if (atoi(line) == friendLists[userIdx].friendCount + 2) return;

    int indices[100], cnt = 0;
    int sendAll = 0;

    char *token = strtok(line, " \n");
    while (token) {
        int x = atoi(token);
        if (x == friendLists[userIdx].friendCount + 1) {
            sendAll = 1;
            break;
        }
        indices[cnt++] = x - 1;
        token = strtok(NULL, " \n");
    }

    get_time_str(timeStr);

    /* Send to all */
    if (sendAll) {
        for (int i = 0; i < friendLists[userIdx].friendCount; i++) {
            char filename[64];
            sprintf(filename, "%s.txt", friendLists[userIdx].friends[i]);
            FILE *fp = fopen(filename, "a");
            if (!fp) continue;

            fprintf(fp,
                "0[%s] From: %s\n%s\n",
                timeStr,
                users[userIdx].username,
                content);

            fclose(fp);
        }
        printf("Message sent to All\n");
        return;
    }

    /* Send selected (support 1 3) */
    for (int i = 0; i < cnt; i++) {
        int idx = indices[i];
        if (idx < 0 || idx >= friendLists[userIdx].friendCount) continue;

        char filename[64];
        sprintf(filename, "%s.txt", friendLists[userIdx].friends[idx]);
        FILE *fp = fopen(filename, "a");
        if (!fp) continue;

        fprintf(fp,
            "0[%s] From: %s\n%s\n",
            timeStr,
            users[userIdx].username,
            content);

        fclose(fp);
        printf("Message sent to %s\n",
               friendLists[userIdx].friends[idx]);
    }
}

/* ================= Read Messages ================= */

void read_messages(int userIdx) {
    char filename[64];
    sprintf(filename, "%s.txt", users[userIdx].username);

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("No messages found.\n");
        return;
    }

    /* ====== 关键：先检查是否有至少一条消息 ====== */
    char testHeader[256];
    if (!fgets(testHeader, sizeof(testHeader), fp)) {
        /* 文件存在，但里面没有任何消息 */
        printf("No messages found.\n");
        fclose(fp);
        return;
    }

    /* 有消息，回到文件开头 */
    rewind(fp);

    /* ====== 显示子菜单 ====== */
    printf("1. Read all messages\n");
    printf("2. Read unread messages only\n");
    printf("3. Back\n");
    printf("Choose an option: ");

    int option;
    scanf("%d", &option);
    getchar();

    if (option == 3) {
        fclose(fp);
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    char header[256], content[256];
    int found = 0;

    while (fgets(header, sizeof(header), fp)) {
        if (!fgets(content, sizeof(content), fp)) break;

        int status;
        char *displayHeader;

        /* 是否有状态位 */
        if (header[0] == '0' || header[0] == '1') {
            status = header[0] - '0';
            displayHeader = header + 1;
        } else {
            status = 0;           /* 老消息默认未读 */
            displayHeader = header;
        }

        /* 显示逻辑 */
        if (option == 1 || (option == 2 && status == 0)) {
            printf("%s", displayHeader);
            printf("%s", content);
            found = 1;
        }

        /* 规则修正：读“所有消息”也应标记为已读 */
        if ((option == 1 || option == 2) && status == 0) {
            status = 1;
        }

        fprintf(temp, "%d%s", status, displayHeader);
        fprintf(temp, "%s", content);
    }

    fclose(fp);
    fclose(temp);

    remove(filename);
    rename("temp.txt", filename);

    if (!found) {
        printf("Found no messages.\n");
    }
}

/* ================= Delete Messages ================= */

void delete_messages(int userIdx) {
    char filename[64];
    sprintf(filename, "%s.txt", users[userIdx].username);

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("No messages found.\n");
        return;
    }

    /* ====== 关键修正：先检查是否有消息 ====== */
    char testHeader[256];
    if (!fgets(testHeader, sizeof(testHeader), fp)) {
        /* 文件存在，但没有任何消息 */
        printf("No messages found.\n");
        fclose(fp);
        return;
    }

    /* 有消息，回到文件开头 */
    rewind(fp);

    /* ====== 开始真正的删除流程 ====== */
    char start[16], end[16], sender[MAX_NAME_LEN];
    char start2[16], end2[16];

    printf("Enter start date (dd/mm/yyyy): ");
    scanf("%s", start);
    printf("Enter end date (dd/mm/yyyy): ");
    scanf("%s", end);
    printf("Enter account name (or 'all'): ");
    scanf("%s", sender);
    getchar();

    convert_date(start, start2);
    convert_date(end, end2);

    FILE *temp = fopen("temp.txt", "w");
    char header[256], content[256];
    int removed = 0;

    while (fgets(header, sizeof(header), fp)) {
        if (!fgets(content, sizeof(content), fp)) break;

        char date[16], from[MAX_NAME_LEN];

        /* 跳过状态位，取日期 */
        sscanf(header, "%*c[%15[^ ]", date);
        sscanf(header, "%*[^F]From: %s", from);

        int inRange =
            strcmp(date, start2) >= 0 &&
            strcmp(date, end2) <= 0;

        int senderMatch =
            strcmp(sender, "all") == 0 ||
            strcmp(sender, from) == 0;

        if (inRange && senderMatch) {
            removed++;
            continue;   /* 不写回 → 删除 */
        }

        fprintf(temp, "%s", header);
        fprintf(temp, "%s", content);
    }

    fclose(fp);
    fclose(temp);

    remove(filename);
    rename("temp.txt", filename);

    printf("Removed %d message(s) from %s in period %s - %s.\n",
           removed, sender, start, end);
}
