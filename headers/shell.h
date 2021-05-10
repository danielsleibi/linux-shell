
#define DEBUG 1
#define MAX_LINE_LEN 4096
#define MAX_ARGS 128
#define END_OF_LINE 0
#define SEQ_OP ';'
#define SEQUENCE 1
#define PIPE_OP '|'
#define PIPE 2


struct cmd_t {
    struct cmd_t* next;
    int          terminator;
    char* exe_path;
    int          nargs;
    char* arg[MAX_ARGS];
};

char* skip_to_non_ws(char* p);
char* skip_to_ws_or_sep(char* p);
struct cmd_t* parse_commands(char* line);
void execute(struct cmd_t* clist);
void free_commands(struct cmd_t* clist);
char* get_command(char* buf, int	size, FILE* in);
void handle_pwd(struct cmd_t* c);
void handle_cd(struct cmd_t* c);
void handle_ps(struct cmd_t* c);
void handle_kill(struct cmd_t* c);