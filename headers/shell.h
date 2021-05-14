
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

/**
 * @brief Skip to first found alphabet or number
 *
 * @param p the char array
 * @return char* the pointer to the alphabet, or NULL on not found
 */
char* skip_to_non_ws(char* p);
/**
 * @brief Skip to first found seprator or space
 *
 * @param p
 * @return char* char* the pointer to the sep or space line, or NULL on not found
 */
char* skip_to_ws_or_sep(char* p);
/**
 * @brief Parse command line into cmd_t structs
 *
 * @param line the line to parse
 * @return struct cmd_t*  pointer to dynamically allocated cmd_t
 */
struct cmd_t* parse_commands(char* line);
/**
 * @brief execute command or command linkedlist
 *
 * @param clist the command(s)
 */
void execute(struct cmd_t* clist);
/**
 * @brief free command or command list
 *
 * @param clist the command(s)
 */
void free_commands(struct cmd_t* clist);
/**
 * @brief Get the command object
 *
 * @param buf
 * @param size
 * @param in
 * @return char*
 */
char* get_command(char* buf, int	size, FILE* in);
/**
 * @brief Handle pwd command if c = "pwd"
 *
 * @param c the command
 */
void handle_pwd(struct cmd_t* c);
/**
 * @brief Handle cd command if c = "cd"
 *
 * @param c the command
 */
void handle_cd(struct cmd_t* c);
/**
 * @brief Handle ps command if c = "ps"
 *
 * @param c the command
 */
void handle_ps(struct cmd_t* c);
/**
 * @brief Handle kill command if c = "kill"
 *
 * @param c the command
 */
void handle_kill(struct cmd_t* c);