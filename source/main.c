#include "../include/first_parse.h"
#include "../include/second_parse.h"
#include "../include/symTable.h"
#include "../include/helper_functions.h"
#include "../include/utils.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

bool assembler(char * file_name){
    bool ret_val = true;
    char new_name[strlen(file_name) + 4]; 
    
    /* ------------ get macro parsed name ------------ */
    strcpy(new_name, file_name);
    strcat(new_name, ".as");
    /* ------------ get macro parsed name ------------ */
    /* stage 1 of macro parse  */
    macro_proccess(new_name);
    
    return ret_val;
}

int main(int argc, char**argv){
    // char * res = (char *)malloc(sizeof(char) * 21); 
    // word_to_string(283585, &res);
    // printf("Resualt shoudld be 0100 | 0101 | 0011 | 1100 | 0001 : %s", res);
    // get_mode("sub R1 , r2");
    // printf("--------\n");
    // get_mode("sub r1  ,   ,  HELLO");
    // get_mode("   ;");
    bool r = handle_cmp("cmp r1, r2");
    printf("Returned: %i", r == true);
    // int i;
    // if (argc == 1){
    //     printf("Not enough args\n");
    //     exit(EXIT_FAILURE);
    // }
    // for (i = 1; i < argc; ++i) {
    //     hcreate(1024);
    //     assembler(argv[i]);
    //     hdestroy();
    // }
    return EXIT_SUCCESS;
}