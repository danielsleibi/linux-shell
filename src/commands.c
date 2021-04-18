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
#include <signal.h>
#include "../headers/commands.h"
#include "../headers/ansicode.h"

char signals[SIGNALS][7] = {
      {"HUP\0"}, {"INT\0"}, {"QUIT\0"},
       {"ILL\0"}, {"TRAP\0"}, {"ABRT\0"},
        {"BUS\0"}, {"FPE\0"},{"KILL\0"},
        {"USR1\0"}, {"SEGV\0"}, {"USR2\0"},
        {"PIPE\0"},{"ALRM\0"},{"TERM\0"},
        {"STKFLT\0"}, {"CHLD\0"}, {"CONT\0"},
        {"STOP\0"}, {"TSTP\0"}, {"TTIN\0"},
        {"TTOU\0"}, {"URG\0"}, {"XCPU\0"},
        {"XFSZ\0"}, {"VTALRM\0"}, {"PROF\0"},
        {"WINCH\0"}, {"POLL\0"}, {"PWR\0"},
         {"SYS\0"}
};

int pwd() {
    char ansi_reset[ANSI_MAX_CODE_SIZE];
    generate_code(ansi_reset, ANSI_MOD_RESET, ANSI_CLR_RESET);
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) {
        perror("commands:getcwd");
        return -1;
    }
    printf("%s%s\n", ansi_reset, cwd);
    return SUCCESS;
}
int cd(const char* path) {
    const char* p;

    if (path == NULL_P) {
        char username[MAX_USERID_LENGTH];
        if (getlogin_r(username, MAX_USERID_LENGTH)) {
            perror("commands:getlogin_r");
            return RETRIEVE_USERNAME_ERRNO;
        }
        char temp[sizeof("/home/") + MAX_USERID_LENGTH];
        strcpy(temp, "/home/");
        strcat(temp, username);
        p = temp;
    } else {
        p = path;
    }
    if (chdir(p) == -1) {
        char error_message[PATH_MAX];
        char ansi_red[ANSI_MAX_CODE_SIZE];
        generate_code(ansi_red, ANSI_MOD_RESET, ANSI_CLR_RED);
        strcpy(error_message, ansi_red);
        strcat(error_message, "commands:chdir");
        perror(error_message);
        return CHDIR_ERRNO;
    }
    return SUCCESS;
}
int ps(int all) {
    char error_message[PATH_MAX];
    char ansi_red[ANSI_MAX_CODE_SIZE];
    generate_code(ansi_red, ANSI_MOD_RESET, ANSI_CLR_RED);
    DIR* dir;
    struct dirent* ent;
    int i, fd_self, fd;
    unsigned long time, stime;
    char flag, * tty;
    char cmd[256], tty_self[256], path[256], time_s[256];
    FILE* file;


    dir = opendir("/proc");
    if (dir == NULL) {
        strcpy(error_message, ansi_red);
        strcat(error_message, "commands:opendir");
        perror(error_message);
        return -4;
    }
    fd_self = open("/proc/self/fd/0", O_RDONLY);
    if (fd_self == -1) {
        strcpy(error_message, ansi_red);
        strcat(error_message, "commands:open");
        perror(error_message);

        return -5;
    }
    sprintf(tty_self, "%s", ttyname(fd_self)); // Store terminal name of current proccess
    printf(PS_FORMAT, "PID", "TTY", "TIME", "CMD");

    while ((ent = readdir(dir)) != NULL) {
        flag = 1;

        // Check if file name is digit or not
        for (i = 0; ent->d_name[i]; i++)
            if (!isdigit(ent->d_name[i])) {
                flag = 0;// it's not a process
                break; // go to next file
            }

        if (flag) {
            sprintf(path, "/proc/%s/fd/0", ent->d_name);
            fd = open(path, O_RDONLY);
            if (fd == -1) {
                strcpy(error_message, ansi_red);
                strcat(error_message, "commands:open");
                perror(error_message);
                return -6;
            }
            tty = ttyname(fd);

            if (tty != NULL) {
                perror("ttyname");
                return -7;
            }
            if (strcmp(tty, tty_self) == 0) {
                sprintf(path, "/proc/%s/stat", ent->d_name);
                file = fopen(path, "r");
                if (file == NULL) {
                    strcpy(error_message, ansi_red);
                    strcat(error_message, "commands:fopen");
                    perror(error_message);
                    return -8;
                }
                // Skip pid(1), state(3), ppid(4), pgrp(5)  
                fscanf(file, "%d%s%c%c%c", &i, cmd, &flag, &flag, &flag);
                cmd[strlen(cmd) - 1] = '\0';
                // Skip stats 1-13 and store stat 14 (which is utime)
                /* utime: Amount of time that this process has been scheduled
                 * in user mode, measured in clock ticks (divide by
                 * sysconf(_SC_CLK_TCK)).
                 */
                for (i = 0; i < 11; i++)
                    fscanf(file, "%lu", &time);
                // store stime(15): Amount of time that this process has been scheduled
                // in kernel mode
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

int psTest() {
    DIR* dir;
    struct dirent* ent;
    int i, fd_self, fd;
    unsigned long time, stime;
    char flag, * tty;
    char cmd[256], tty_self[256], path[256], time_s[256];
    FILE* file;

    dir = opendir("/proc");
    fd_self = open("/proc/self/fd/0", O_RDONLY);
    sprintf(tty_self, "%s", ttyname(fd_self));
    printf(PS_FORMAT, "PID", "TTY", "TIME", "CMD");

    while ((ent = readdir(dir)) != NULL) {
        flag = 1;

        // Check if file name is digit or not
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
    return 0;
}

int send_signal(int pid, int signal, int list) {
    char ansi_reset[ANSI_MAX_CODE_SIZE];
    generate_code(ansi_reset, ANSI_MOD_RESET, ANSI_CLR_RESET);
    if (list) {
        for (int i = 0; i < SIGNALS; i++) {
            printf("%s%s ", ansi_reset, signals[i]);
        }
        printf("\n");
    }
    if (pid == NULL_P)
        return SUCCESS;

    int sig = SIGTERM;
    if (signal > SIGNALS || signal < NULL_P) {
        return UNKNOWN_SIG_ERRNO;
    }
    if (signal != NULL_P)
        sig = signal;
    printf("%sSending %s to %d\n", ansi_reset, signals[sig - 1], pid);
    if (kill(pid, sig) == -1) {
        perror("\033[0;31mkill");
        return -1;
    }
    return SUCCESS;
}


int send_signal_s(int pid, char* signa, int list) {
    for (int i = 0; i < SIGNALS; i++) {
        if (strcmp(signa, signals[i]) == 0) {
            return send_signal(pid, i + 1, list);
        }
    }
    return UNKNOWN_SIG_ERRNO;
}


// int main() {
//     int pid = fork();
//     if (pid == -1) {
//         return -1;
//     }
//     if (pid == 0) {
//         sleep(1);
//         return 0;
//     }
//     int r = send_signal_s(pid, "USR21", 0);
//     waitpid(pid);
//     printf("\033[0mResult: %d\n", r);
//     return SUCCESS;
// }