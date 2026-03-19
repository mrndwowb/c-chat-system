#include <stdio.h>
#include "user.h"
#include "friends.h"
#include "message.h"

/* Clear remaining input to avoid scanf/fgets issues */
static void clear_stdin_line(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
}

static void login_menu(void);
static void main_menu(int userIdx);

int main(void) {
    /* Load data once at program start */
    load_users();
    load_friends();

    login_menu();
    return 0;
}

static void login_menu(void) {
    int option;

    while (1) {
        printf("\n==================== Login ====================\n");
        printf("1. Login\n");
        printf("2. Register (If you do not have an account)\n");
        printf("3. Exit\n");
        printf("===============================================\n");
        printf("Choose an option (1-3): ");

        if (scanf("%d", &option) != 1) {
            clear_stdin_line();
            printf("Invalid choice! Please enter a number between 1 and 3.\n");
            continue;
        }
        clear_stdin_line();

        if (option < 1 || option > 3) {
            printf("Invalid choice! Please enter a number between 1 and 3.\n");
            continue;
        }

        
        if (option == 1) {
            int userIdx = login_user();
            if (userIdx != -1) {
                main_menu(userIdx);
            }
            /* No matter login success/fail, go back to Login menu */
        } else if (option == 2) {
            register_user();
            /* After register, back to Login menu */
            load_users();   /* refresh in-memory users */
            load_friends(); /* ensure friendLists size aligns with userCount */
        } else if (option == 3) {
            printf("Byebye!\n");
            printf("Press any key to continue...\n");
            getchar();   /* wait for key */
            return;     /* exit program */
        }
    }
}

static void main_menu(int userIdx) {
    int option;

    while (1) {
        printf("\n==================== Main Service ====================\n");
        printf("1. Manage friends\n");
        printf("2. Manage messages\n");
        printf("3. Back\n");
        printf("======================================================\n");
        printf("Choose an option (1-3): ");

        if (scanf("%d", &option) != 1) {
            clear_stdin_line();
            printf("Invalid option number!\n");
            continue;
        }
        clear_stdin_line();

        if (option == 1) {
            manage_friends(userIdx);
        } else if (option == 2) {
            manage_messages(userIdx);
        } else if (option == 3) {
            return; /* Back to Login menu */
        } else {
            printf("Invalid option number!\n");
        }
    }
}
