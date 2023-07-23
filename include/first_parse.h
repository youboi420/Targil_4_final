#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <string.h>
#include <stdbool.h>
#define LIMIT 82

typedef enum MODES {LABEL = 100, MOV, CMP, ADD, SUB, LEA, CLR, NOT, INC, DEC, JMP, BNE, JSR, RED, PRN, RTS, STOP} MODES;

bool check_macro_is_defined(char * macro_name);
int check_inst(char * buffer);
char* get_macro_name(char * buffer, int mode);
int macro_proccess(char *file_name);