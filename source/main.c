#include "../include/first_parse.h"
#include "../include/second_parse.h"
#include "../include/helper_functions.h"
#include "../include/utils.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int EXPORT_FILES = 1, IC, DC;

bool assembler(char * file_name){
    int res;
    bool ret_val = true;
    char macro_parsed[strlen(file_name) + 4]; 
    symPTR * root = (symPTR*)malloc(sizeof(symNode));
    IC = DC = 0;
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
            case MACRO_EXCEPRION:
            case LINE_EXCEPTION:
            case FILE_EXCEPTION:
            default:
                ret_val = false;
                break;
        }
    }

    if (ret_val){    
        printf("---------STARTED STAGE 2 PARSE 1---------------\n");
        res = parse2_file_stage_1(macro_parsed, &IC, &DC, root);
        printf("---------ENDED STAGE 2 PARSE 1 STOPPED: %i | PASSED: %i---------------\n\n", res == STOP, res == PASSED);
        print_sym_list(root);
        printf("IC: %i | DC: %i", IC,DC);
    }    
    if(EXPORT_FILES){
        printf("---------STARTED STAGE 2 PARSE 2---------------\n");
        res = parse2_file_stage_2(macro_parsed, &IC, &DC, EXPORT_FILES, root);
        printf("---------ENDED STAGE 2 PARSE 2 STOPPED: %i | PASSED: %i---------------\n\n", res == STOP, res == PASSED);
    }
    printf("------------------------\n\n");
    print_sym_list(root);
    printf("------------------------\n\n");
    destroy_symTable(root);
    return ret_val;
}

int main(int argc, char**argv){
    /* int i;
    if (argc == 1){
        printf("Not enough args\n");
        exit(EXIT_FAILURE);
    }
    for (i = 1; i < argc; ++i) {
        assembler(argv[i]);
    }
    return EXIT_SUCCESS; */
}