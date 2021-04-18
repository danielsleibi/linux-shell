#include <stdio.h>
#include  <stdlib.h>
#define MAX_LINE_LEN 4096
#define MAX_ARGS 128
#define SEQ_OP ';'


struct cmd_t {
    char* file_name;
    char* args[MAX_ARGS];
    int nargs;
    struct cmd_t* next;
};

char* next_arg(char* ptr) {
    while (*ptr != '\0') {
        if (*ptr != ' ' && *ptr != '\t' && *ptr != '\n') {
            return ptr;
        }
        ptr++;
    }
    return 0;// REACHED END
}


char* next_space(char* ptr) {
    while (*ptr != '\0') {
        if (*ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == SEQ_OP) {
            return ptr;
        }
        ptr++;
    }
    return 0;// REACHED END
}

int main() {
    char cmd[MAX_LINE_LEN];
    fgets(cmd, sizeof(cmd), stdin);
    printf("cmd: \033[0;32m%s\033[0m\n", cmd);


    /* parse */
    char* ptr = cmd;
    struct cmd_t* cmd_v;
    cmd_v = malloc(sizeof * cmd_v);
    // Skip to first arg
    ptr = next_arg(ptr);
    cmd_v->file_name = ptr;

    while (*ptr) {
        ptr = next_space(ptr);
        if (!ptr)continue;


    }

    printf("file_name: \033[0;32m%s", cmd_v->file_name);
    free(cmd_v);

    return 0;
}