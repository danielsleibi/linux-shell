#define ANSI_MOD_RESET 0
#define ANSI_MOD_BOLD 1
#define ANSI_MOD_DIM 2
#define ANSI_MOD_ITALIC 3
#define ANSI_MOD_UNDERLINE 4
#define ANSI_MOD_BLINKING 5
#define ANSI_MOD_INVERSE 7
#define ANSI_MOD_INVISIBLE 8
#define ANSI_MOD_STRIKETHROUGH 9
#define ANSI_CLR_BLACK 30
#define ANSI_CLR_RED 31
#define ANSI_CLR_GREEN 32
#define ANSI_CLR_YELLOW 33
#define ANSI_CLR_BLUE 34
#define ANSI_CLR_MAGNETA 35
#define ANSI_CLR_CYAN 36
#define ANSI_CLR_WHITE 37
#define ANSI_CLR_RESET 0
#define ANSI_MAX_CODE_SIZE sizeof("\033[%d;%dm")

void generate_code(char* ref, int mod, int color);