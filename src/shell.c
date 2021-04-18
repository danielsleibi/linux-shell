#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
#include "../headers/shell.h"
#include "../headers/commands.h"
#include "../headers/ansicode.h"

char ansi_red[ANSI_MAX_CODE_SIZE]; // Use for errors
char ansi_red_italic[ANSI_MAX_CODE_SIZE]; // Use for info
char ansi_reset[ANSI_MAX_CODE_SIZE]; // reset


void main(void) {
    generate_code(ansi_red, ANSI_MOD_RESET, ANSI_CLR_RED);
    generate_code(ansi_red_italic, ANSI_MOD_ITALIC, ANSI_CLR_RED);
    generate_code(ansi_reset, ANSI_MOD_RESET, ANSI_MOD_RESET);

    char linebuf[MAX_LINE_LEN];
    struct cmd_t* commands;

    while (get_command(linebuf, MAX_LINE_LEN, stdin) != NULL) {
        commands = parse_commands(linebuf);
        if (commands) {
            execute(commands);
            free_commands(commands);
        }
    }
}



void* ck_malloc(size_t   size) {
    void* ret = malloc(size);
    if (!ret) {
        perror("shell:malloc");
        exit(1);
    }
    return ret;
}

char* skip_to_non_ws(char* p) {
    int    ch;
    while (ch = *p) {
        if (ch != ' ' && ch != '\t' && ch != '\n') return p;
        ++p;
    }
    return 0;
}

char* skip_to_ws_or_sep(char* p) {
    int    ch;
    while (ch = *p) {
        if (ch == ' ' || ch == '\t' || ch == '\n') return p;
        if (ch == SEQ_OP) return p;
        ++p;
    }
    return 0;
}

struct cmd_t* parse_commands(char* line) {
    char* ptr;
    int ix;
    struct cmd_t* cur;

    ptr = skip_to_non_ws(line);
    if (!ptr) return 0;
    cur = ck_malloc(sizeof * cur);
    cur->next = 0;
    cur->exe_path = ptr;
    cur->arg[0] = ptr;
    cur->terminator = END_OF_LINE;
    ix = 1;
    for (;;) {
        ptr = skip_to_ws_or_sep(ptr);
        if (!ptr) {
            break;
        }
        if (*ptr == SEQ_OP) {
            *ptr = 0;
            cur->next = parse_commands(ptr + 1);
            if (cur->next) {
                cur->terminator = SEQUENCE;
            }
            break;
        }
        *ptr = 0;
        ptr = skip_to_non_ws(ptr + 1);
        if (!ptr) {
            break;
        }
        if (*ptr == SEQ_OP) {
            /* found a sequence operator */
            cur->next = parse_commands(ptr + 1);
            if (cur->next) {
                cur->terminator = SEQUENCE;
            }
            break;
        }
        cur->arg[ix] = ptr;
        ++ix;
    }
    cur->arg[ix] = 0; cur->nargs = ix;
    return cur;
}

void   execute(struct cmd_t* clist) {
    int    pid, npid, stat;

    if (!strcmp(clist->exe_path, "cd")) {
        const char* path = clist->arg[1];
        if (!path) {
            cd(NULL_P);
        } else
            cd(path);
    } else if (!strcmp(clist->exe_path, "exit")) {
        exit(0);
    } else {
        pid = fork();
        if (pid == -1) {
            perror("shell:fork");
            exit(1);
        }
        if (!pid) {
            /* child */

            if (!strcmp(clist->exe_path, "pwd")) {
                pwd();
                exit(0);
            } else  if (!strcmp(clist->exe_path, "kill")) {
                int flag = 1;
                for (int i = 0; clist->arg[1][i]; i++)
                    if (!isdigit(clist->arg[1][i])) {
                        flag = 0;
                        break;
                    }

                if (clist->nargs == 3) {
                    for (int i = 0; clist->arg[2][i]; i++)
                        if (!isdigit(clist->arg[2][i])) {
                            printf("%sInvalid pid\n", ansi_red);
                            exit(0);
                        }
                    if (flag) {
                        int r = send_signal(atoi(clist->arg[2]), atoi(clist->arg[1]), 0);
                        if (r == UNKNOWN_SIG_ERRNO) {
                            printf("%sUnknown signal: use kill -l\n", ansi_red_italic);
                        }
                    } else {
                        int r = send_signal_s(atoi(clist->arg[2]), clist->arg[1], 0);
                        if (r == UNKNOWN_SIG_ERRNO) {
                            printf("%sUnknown signal: use kill -l\n", ansi_red_italic);
                        }
                    }

                } else if (clist->nargs == 2) {
                    if (!strcmp(clist->arg[1], "-l")) {
                        send_signal(NULL_P, NULL_P, 1);
                        exit(0);
                    }
                    if (!flag) {
                        printf("%sInvalid pid\n", ansi_red);
                        exit(0);
                    }
                    int r = send_signal(atoi(clist->arg[1]), NULL_P, 0);
                    if (r == UNKNOWN_SIG_ERRNO) {
                        printf("%sUnknown signal: use kill -l\n", ansi_red_italic);
                    }
                } else {
                    printf("%sInvalid args: use kill signal pid\n", ansi_red_italic);
                }
                exit(0);
            }
            execvp(clist->exe_path, clist->arg);
            fprintf(stderr, "%sCommand undefined: %s\n", ansi_red_italic, clist->exe_path);
            exit(1);
        }

        do {


            npid = wait(&stat);
            printf("%sProcess %d exited with status %d\n", ansi_reset, npid, stat);
        } while (npid != pid);
    }
    switch (clist->terminator) {
    case SEQUENCE:
        execute(clist->next);
    }
}

void   free_commands(struct cmd_t* clist) {
    struct cmd_t* nxt;
    do {
        nxt = clist->next;
        free(clist);
        clist = nxt;
    } while (clist);
}

char* get_command(char* buf, int	size, FILE* in) {
    char ansi_green_bold[ANSI_MAX_CODE_SIZE];
    char ansi_blue_bold[ANSI_MAX_CODE_SIZE];
    char ansi_white_bold[ANSI_MAX_CODE_SIZE];
    char ansi_green[ANSI_MAX_CODE_SIZE];
    generate_code(ansi_green_bold, ANSI_MOD_BOLD, ANSI_CLR_GREEN);
    generate_code(ansi_blue_bold, ANSI_MOD_BOLD, ANSI_CLR_BLUE);
    generate_code(ansi_white_bold, ANSI_MOD_BOLD, ANSI_CLR_WHITE);
    generate_code(ansi_green, ANSI_MOD_RESET, ANSI_CLR_GREEN);
    char username[MAX_USERID_LENGTH];
    char cwd[MAX_LINE_LEN];
    if (getlogin_r(username, MAX_USERID_LENGTH)) {
        char error_message[MAX_LINE_LEN];
        strcpy(error_message, ansi_red);
        strcat(error_message, "getlogin_r");
        perror(error_message);
        strcpy(username, "UNKNOWN");
    }

    if (!getcwd(cwd, sizeof(cwd))) {
        strcpy(cwd, "UNKNOWN");
    }
    char str[MAX_LINE_LEN];

    sprintf(str, "\n%s(%s%s%s) - %s%s%s\n>>>%s ",
        ansi_green_bold, ansi_blue_bold, username,
        ansi_green_bold, ansi_white_bold, cwd,
        ansi_blue_bold, ansi_green);
    if (in == stdin) {
        fputs(str, stdout);
    }
    return fgets(buf, size, in);
}