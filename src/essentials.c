#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../headers/essentials.h"
#include "../headers/ansicode.h"

#define MAX_MESSAGE_LENGTH 4096
#define INTEGER 1
#define NOT_INTEGER 0

int e_isinteger(char* s) {
    for (int i = 0;s[i]; i++)
        if (!isdigit(s[i])) {
            return NOT_INTEGER;
        }
    return INTEGER;
}

void e_perror(char* s) {
    // Error checking
    if (strlen(s) > MAX_MESSAGE_LENGTH) {
        fprintf(stderr, "Failed to perror, MESSAGE LENGTH exceeded\n");
        return;
    }

    // Generate message color
    char error_message[MAX_MESSAGE_LENGTH + ANSI_MAX_CODE_SIZE];
    char ansi_red[ANSI_MAX_CODE_SIZE];
    generate_code(ansi_red, ANSI_MOD_RESET, ANSI_CLR_RED);

    // Blend message with color
    strcpy(error_message, ansi_red);
    strcat(error_message, s);
    // Print
    perror(error_message);
}

void e_print_error(char* s) {
    // Generate message color
    char ansi_red[ANSI_MAX_CODE_SIZE];
    generate_code(ansi_red, ANSI_MOD_RESET, ANSI_CLR_RED);

    // Print
    printf("%s%s\n", ansi_red, s);
}


void e_print_info(char* s) {
    // Generate message color
    char ansi_red_italic[ANSI_MAX_CODE_SIZE];
    generate_code(ansi_red_italic, ANSI_MOD_ITALIC, ANSI_CLR_RED);

    // Print
    printf("%s%s\n", ansi_red_italic, s);
}


void* e_malloc(size_t size) {
    void* mem = malloc(size);
    if (!mem) {
        perror("essentials:malloc");
        exit(1);
    }
    return mem;
}

int e_exists(const char* fname) {
    FILE* file;
    if ((file = fopen(fname, "r"))) {
        fclose(file);
        return 1;
    }
    return 0;
}