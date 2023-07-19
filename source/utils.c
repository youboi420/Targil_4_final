#include "../include/utils.h"
#include "../include/first_parse.h"
#include <ctype.h>
#include <search.h>
#include <stdio.h>
#include <string.h>
// this is a test
// this is another test

char *MODES_STR[] = {
    "LABEL",
    "MOV",
    "CMP",
    "ADD",
    "SUB",
    "LEA",
    "CLR",
    "NOT",
    "INC",
    "DEC",
    "JMP",
    "BNE",
    "JSR",
    "RED",
    "PRN",
    "RTS",
    "STOP",
    "COMMENT"
};

int check_mem_mode(char *buffer){
    int i = 0, ret_val = 0, index = 0, start, end, comma_count = 0;
    char * reg_name = NULL, *buffer_to_check = (char*)malloc(strlen(buffer) + 2),str[4], reg[4];
    strcpy(buffer_to_check, buffer);
    strcat(buffer_to_check, " ");
    if (strstr(buffer_to_check, "stop") == NULL){
        //        , sub r1, r2
        while(isspace(buffer_to_check[index]) && buffer_to_check[index] != ',' )++index;
        if (buffer_to_check[index] == ',') return COMMENT;
        index += 3;
        while(isspace(buffer_to_check[index])) ++index;
        if (buffer_to_check[index] == ',') return COMMENT;
        
        for(i = 0; i < 3; ++i){
            if (buffer_to_check[index + i] != ',' && buffer_to_check[index + i] != ' ') reg[i] = buffer_to_check[index + i];
            else break;
        }
        
        reg[i] = '\0';

        index += i;
        if (buffer_to_check[index] == ',' || buffer_to_check[index] == ' ') {
            for (i = 0; i < 16; ++i) {
            reg_name = malloc(sizeof(char) * 4);
            strcpy(reg_name, "r");
            sprintf(str, "%d", i);
            strcat(reg_name, str);
            if (strcmp(reg_name, reg) == 0){
                ret_val = 1;
                free(reg_name);
                break;
            }
            free(reg_name);
            }
        }
        else while((isspace(buffer_to_check[index]) || buffer_to_check[index] == ',') && comma_count > 1){
            if (buffer_to_check[index] == ',') ++comma_count;
            ++index;
        }
        
        if (comma_count > 0) {
            printf("multple consecutive commas\n");
            return COMMENT;
        }

        while(isspace(buffer_to_check[index])) ++index;
        ++index;
        while(isspace(buffer_to_check[index])) ++index;

        for(i = 0; i < 3; ++i){
            if (buffer_to_check[index + i] != ',' && buffer_to_check[index + i] != ' ') reg[i] = buffer_to_check[index + i];
            else break;
        }
        index += i;
        if (buffer[index] == ',' ) ret_val = COMMENT;
        if (buffer_to_check[index] == ' ' || buffer_to_check[index] == '\n' || buffer_to_check[index] == '\0') {
            for (i = 0; i < 16; ++i) {
            reg_name = malloc(sizeof(char) * 4);
            strcpy(reg_name, "r");
            sprintf(str, "%d", i);
            strcat(reg_name, str);
            if (strcmp(reg_name, reg) == 0){
                ret_val =  ret_val && 1;
                free(reg_name);
                break;
            }
            free(reg_name);
            }
        }
        else{
            if (ret_val != COMMENT)
                ret_val = 0;
        }
    }
    else{
        ret_val = STOP;
    }
        
    return  ret_val;
}

int get_mode(char * instr){
    int resault = check_inst(instr);
    int regs_mode = check_mem_mode(instr);
    if (resault != COMMENT){
        if (regs_mode == 1){
            printf("[1] Internal memory mode\n");
        }
        else if (regs_mode == COMMENT){
            printf(" , :(\n");
        }
        else if (regs_mode == STOP){
            printf("Shoudld stop the asm prog\n");
        }
        else if (regs_mode == 0){
            printf("[0] External/Relocatable memory mode\n");
        }
    }
    else{
        printf("This is a comment should ignore\n");
    }
        
    /* switch (res) {
        case CMP:
        case PRN:
            // something 0,1,2,3
            
            break;
        case MOV:
        case ADD:
        case SUB:
        case LEA:
        case CLR:
        case NOT:
        case DEC:
        case INC:
        case RED:
            // something 1,2,3
            break;
        case JMP:
        case BNE:
        case JSR:
            // something 1,2
            break;
        case RTS:
        case STOP:
            // None
            break; 
            
    }
    */
    return 1;
}

unsigned int instruction_to_word_A(char * instr){
    // sub r1, HELLO
    // opcode 2 
    // funct 11
    // 0 ARE
    // FIRST WORD  0001 | 0000 | 0000 | 0000 | 0010  
    // SECOND WORD 0100 | 1011 | 0001 | 11 | 0100 | 11
    return 1;
}

void word_to_string(unsigned int n, char **str_p){
    unsigned int copy = n, mask = 1;
    int counter = 0, res = 0;
    int *P_res = &res;
    char resault[21];
    while(counter < 20){
        resault[19 - counter] = (copy & mask) ? '1' : '0';
        copy |= (copy - 1);
        mask <<= 1;
        ++counter;
    }
    resault[20] = '\0';
    strcpy(*str_p, resault);
}

LINE_TYPE check_line_type(char *buffer){
    int index = 0, ret_val = OTHER;
    if (!buffer) return ret_val;
    while (isspace(buffer[index])) ++index;
    if( buffer[index] == '\n' ) ret_val = EMPTY;
    return ret_val;
}