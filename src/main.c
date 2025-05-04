#include "../include/vfs.h"
#include <stdio.h>
#include <string.h>

#define CMD_SIZE 128
#define ARG_SIZE 64

int main() {
    init_fs();

    char cmd[CMD_SIZE];
    char arg1[ARG_SIZE], arg2[ARG_SIZE];

    while (1) {
        printf("> ");
        fflush(stdout);
        int count = scanf("%s", cmd);

        if (count == EOF) break;

        if (strcmp(cmd, "exit") == 0) break;
        else if (strcmp(cmd, "mkdir") == 0) scanf("%s", arg1), mkdir_vfs(arg1);
        else if (strcmp(cmd, "touch") == 0) scanf("%s", arg1), touch_vfs(arg1);
        else if (strcmp(cmd, "ls") == 0) ls_vfs();
        else if (strcmp(cmd, "cd") == 0) scanf("%s", arg1), cd_vfs(arg1);
        else if (strcmp(cmd, "pwd") == 0) pwd_vfs();
        else if (strcmp(cmd, "rmdir") == 0) scanf("%s", arg1), rmdir_vfs(arg1);
        else if (strcmp(cmd, "rm") == 0) scanf("%s", arg1), rm_vfs(arg1);
        else if (strcmp(cmd, "cp") == 0) scanf("%s %s", arg1, arg2), cp_vfs(arg1, arg2);
        else if (strcmp(cmd, "mv") == 0) scanf("%s %s", arg1, arg2), mv_vfs(arg1, arg2);
        else if (strcmp(cmd, "chmod") == 0) {
            int mode; scanf("%s %o", arg1, &mode); chmod_vfs(arg1, mode);
        } else if (strcmp(cmd, "chown") == 0) scanf("%s %s", arg1, arg2), chown_vfs(arg1, arg2);
        else if (strcmp(cmd, "find") == 0) scanf("%s", arg1), find_vfs(arg1);
        else if (strcmp(cmd, "ln") == 0) scanf("%s %s", arg1, arg2), ln_vfs(arg1, arg2);
        else if (strcmp(cmd, "df") == 0) df_vfs();
        else if (strcmp(cmd, "du") == 0) scanf("%s", arg1), du_vfs(arg1);
        else if (strcmp(cmd, "save") == 0) scanf("%s", arg1), save_fs(arg1);
        else if (strcmp(cmd, "load") == 0) scanf("%s", arg1), load_fs(arg1);
        else printf("Unknown command: %s", cmd);

        while (getchar() != '\n');
    }

    return 0;
}
