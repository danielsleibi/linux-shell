
#define DEBUG 1
#define MAX_LINE_LEN 4096
#define MAX_ARGS 128
#define END_OF_LINE 0
#define SEQ_OP ';'
#define SEQUENCE 1


struct cmd {
    struct cmd* next;
    int          terminator;
    char* exe_path;
    int          nargs;
    char* arg[MAX_ARGS];
};

void* ck_malloc(size_t   size);
char* skip_to_non_ws(char* p);
char* skip_to_ws_or_sep(char* p);
struct cmd* parse_commands(char* line);
void   execute(struct cmd* clist);
char* get_command(char* buf, int	size, FILE* in);

char* get_working_dir();