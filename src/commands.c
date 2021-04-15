#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../headers/commands.h"


int pwd() {
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) {
        perror("get cwd");
        return -1;
    }
    printf("%s\n", cwd);
    return SUCCESS;
}
int cd(const char* path) {
    const char* p;

    if (path == NULL_P) {
        char username[MAX_USERID_LENGTH];
        if (getlogin_r(username, MAX_USERID_LENGTH)) {
            perror("getlogin_r");
            return -2;
        }
        char temp[sizeof("/home/") + MAX_USERID_LENGTH];
        strcpy(temp, "/home/");
        strcat(temp, username);
        p = temp;
    } else {
        p = path;
    }
    if (chdir(p)) {
        perror("chdir");
        return -3;
    }


    return SUCCESS;
}
int ps(int all) {

    DIR* dir;
    struct dirent* ent;
    int i, fd_self, fd;
    unsigned long time, stime;
    char flag, * tty;
    char cmd[256], tty_self[256], path[256], time_s[256];
    FILE* file;

    dir = opendir("/proc");
    if (dir == NULL) {
        perror("opendir");
        return -4;
    }
    fd_self = open("/proc/self/fd/0", O_RDONLY);
    if (fd_self == -1) {
        perror("open");
        return -5;
    }
    sprintf(tty_self, "%s", ttyname(fd_self)); // save the terminal name in tty_self
    printf(PS_FORMAT, "PID", "TTY", "TIME", "CMD");
    while ((ent = readdir(dir)) != NULL) {
        flag = 1;
        // Skip to next file name with number (PROCESS)
        for (i = 0; ent->d_name[i]; i++)
            if (!isdigit(ent->d_name[i])) {
                flag = 0;
                break;
            }

        if (flag) {
            sprintf(path, "/proc/%s/fd/0", ent->d_name);
            fd = open(path, O_RDONLY);
            tty = ttyname(fd);

            if (tty && strcmp(tty, tty_self) == 0) {

                sprintf(path, "/proc/%s/stat", ent->d_name);
                file = fopen(path, "r");
                fscanf(file, "%d%s%c%c%c", &i, cmd, &flag, &flag, &flag);
                cmd[strlen(cmd) - 1] = '\0';

                for (i = 0; i < 11; i++)
                    fscanf(file, "%lu", &time);
                fscanf(file, "%lu", &stime);
                time = (int)((double)(time + stime) / sysconf(_SC_CLK_TCK));
                sprintf(time_s, "%02lu:%02lu:%02lu",
                    (time / 3600) % 3600, (time / 60) % 60, time % 60);

                printf(PS_FORMAT, ent->d_name, tty + 5, time_s, cmd + 1);
                fclose(file);
            }
            close(fd);
        }

    }
    close(fd_self);
    return SUCCESS;
}

int kill();

int main() {

    int r = ps(0);


    printf("Result: %d\n", r);
    return SUCCESS;
}