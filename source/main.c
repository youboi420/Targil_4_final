#include "../include/first_parse.h"
#include "../include/second_parse.h"
#include "../include/helper_functions.h"
#include "../include/utils.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int EXPORT_FILES = 1;

bool assembler(char * file_name){
    int res;
    bool ret_val = true;
    char macro_parsed[strlen(file_name) + 4]; 
    /* ------------ get macro parsed name ------------ */
    strcpy(macro_parsed, file_name);
    strcat(macro_parsed, ".am");
    /* ------------ get macro parsed name ------------ */
    
    /* stage 1 of macro parse  */
    printf("---------STARTED STAGE 1 MACRO PARSE---------------\n");
    hcreate(1024);
    res = macro_proccess(file_name);
    hdestroy();
    printf("---------ENDED STAGE 1 MACRO PARSE---------------\n\n");
    if (res != PASSED){
        switch (res) {
            case LINE_EXCEPTION:
                break;
            case FILE_EXCEPTION:
                break;
            case MACRO_EXCEPRION:
                break;
        }
    }
    printf("---------STARTED STAGE 2 PARSE 1---------------\n");
    hcreate(1024);
    int IC = 0, DC = 0;
    res = parse2_file_stage_1(macro_parsed, &IC, &DC);
    printf("---------ENDED STAGE 2 PARSE 1 STOPPED: %i | PASSED: %i---------------\n\n", res == STOP, res == PASSED);

    // printf("---------STARTED STAGE 2 PARSE 2---------------\n");
    // res = parse2_file_stage_2(macro_parsed, &IC, &DC, EXPORT_FILES);
    // printf("---------ENDED STAGE 2 PARSE 2 STOPPED: %i | PASSED: %i---------------\n\n", res == STOP, res == PASSED);
    hdestroy();
    return ret_val;
}

int main(int argc, char**argv){
    
    
    // char * c = word_to_ob_line("01000000010110110001");
    // printf("Resault: %s", c);
    // free(c);
    // char * res = (char *)malloc(sizeof(char) * 21); 
    // word_to_string(283585, &res);
    // printf("Resualt shoudld be 0100 | 0101 | 0011 | 1100 | 0001 : %s", res);
    // get_mode("sub R1 , r2");
    // printf("--------\n");
    // get_mode("sub r1  ,   ,  HELLO");
    // get_mode("   ;");
    
    // bool r = handle_cmp("cmp r1, r2");
    // printf("Returned: %i", r == true);
    
    int i;
    if (argc == 1){
        printf("Not enough args\n");
        exit(EXIT_FAILURE);
    }
    for (i = 1; i < argc; ++i) {
        assembler(argv[i]);
    }
    return EXIT_SUCCESS;
}