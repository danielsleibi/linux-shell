int e_isinteger(char* s);

/**
 * @brief Prints red colored perror messages
 *
 * @param s message, must not exceed length of MAX_MESSAGE_LENGTH
 */
void e_perror(char* s);
/**
 * @brief Prints red colored error messages
 *
 * @param s message
 */
void e_print_error(char* s);
/**
 * @brief Prints red colored italic error messages
 *
 * @param s message
 */
void e_print_info(char* s);
/**
 * @brief Safely allocates dynamic memory with error checking
 *
 * @param size size of memory to allocate in bytes
 * @return void* pointer to the memory allocated
 *
 * @note Uneeded allocated memory must be freed, or it might cause memory leak
 */
void* e_malloc(size_t size);