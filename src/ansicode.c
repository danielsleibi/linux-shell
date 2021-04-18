#include "../headers/ansicode.h"
#include <string.h>
#include <stdio.h>


void generate_code(char* ref, int mod, int color) {
    sprintf(ref, "\033[%d;%dm", mod, color);
}

