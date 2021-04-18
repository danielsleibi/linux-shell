#define SUCCESS 0
#define PATH_MAX 4096
#define MAX_USERID_LENGTH 32
#define NULL_P 0
#define SIGNALS 31
#define PS_FORMAT "%5s %s\t%8s %s\n"
#define GETCWD_ERRNO -1
#define RETRIEVE_USERNAME_ERRNO -1
#define CHDIR_ERRNO -2
#define KILL_ERRNO -1
#define UNKNOWN_SIG_ERRNO -2

/**
 * @brief Prints current working directory
 *
 * @return Error code, 0 if success
 */
int pwd();
/**
 * @brief Changes current working directory
 *
 * @param path location to change current directory to, NULL_P to set location to home dir
 * @return Error code, 0 if success
 * @note cd has to be called in the parent process, else the changes will only happen to the child
 */
int cd(const char* path);
/**
 * @brief Print user processes
 * @param all should print all processes? (-A flag) 0 false, 1 true
 * @return Error code, 0 if success
 */
int ps(int all);
/**
 * @brief Send a signal to a process
 *
 * @param pid id of process to signal, if NULL_P is passed no signal will be sent
 * @param signal signal to send, NULL_P to send SIGTERM
 * @param list should print the signal list that the command can send?, 0 false, 1 true
 * @return Error code, 0 if success
 */
int send_signal(int pid, int signal, int list);
int send_signal_s(int pid, char* signal, int list);

