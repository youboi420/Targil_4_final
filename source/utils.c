#include "../include/helper_functions.h"
#include "../include/utils.h"
#include "../include/first_parse.h"
#include <ctype.h>
#include <regex.h>
#include <search.h>
#include <stdio.h>
#include <string.h>

/* char *MODES_STR[] = {
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
 */
int check_mem_mode(char *buffer){
    int i = 0, ret_val = OTHER, index = 0, start, end, comma_count = 0, flag;
    char * reg_name = NULL, *buffer_to_check = (char*)malloc(strlen(buffer) + 2),str[4], reg[4], skip = 0;
    strcpy(buffer_to_check, buffer);
    strcat(buffer_to_check, " ");
    if (strstr(buffer_to_check, "stop") == NULL){
        //        , sub r1, r2
        while(isspace(buffer_to_check[index]) && buffer_to_check[index] != ',' )++index;
        if (buffer_to_check[index] == ',') return COMMENT;
        index += 3;
        while(isspace(buffer_to_check[index])) ++index;
        if (buffer_to_check[index] == ',') return COMMENT;
        
        if (buffer_to_check[index] == '#') ret_val = direct_addr;
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
                    
                    ret_val = direct_addr;
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

        if (buffer_to_check[index] == '#'){
            flag = skip = 1;
            // ret_val = ret_val & 1;
            if (ret_val != immediate_addr) ret_val = direct_addr;
        }
        for(i = 0; i < 3; ++i){
            if (buffer_to_check[index + i] != ',' && buffer_to_check[index + i] != ' ') reg[i] = buffer_to_check[index + i];
            else break;
        }
        reg[i] = '\0';
        index += i;
        if (buffer[index] == ',' ) ret_val = COMMA;
        if (buffer_to_check[index] == ' ' || buffer_to_check[index] == '\n' || buffer_to_check[index] == '\0' && !skip) {
            flag = 0;
            for (i = 0; i < 16; ++i) {
                reg_name = malloc(sizeof(char) * 4);
                strcpy(reg_name, "r");
                sprintf(str, "%d", i);
                strcat(reg_name, str);
                if (strcmp(reg_name, reg) == 0){
                    flag = 1;
                    free(reg_name);
                    break;
                }
                free(reg_name);
            }
            if (strlen(reg) != 0 && i >= 16 && !skip)  ret_val = OTHER;
            if(strlen(reg) != 0 && flag && i < 16) if(ret_val == direct_addr) ret_val = direct_addr;
        
        }
        else{
            if (ret_val != COMMENT)
                ret_val = 0;
        }
    }
    else{
        ret_val = STOP;
    }
    if (ret_val == 1) ret_val = direct_addr;
    return ret_val;
}

int get_mode(char * instr){
    int ret_val = check_line_type(instr);
    int resault = check_inst(instr);
    int regs_mode = check_mem_mode(instr);
    if (resault != COMMENT && ret_val == OTHER){
        ret_val = regs_mode;
        if (regs_mode == direct_addr){
            printf("[1] Direct memory mode\n");
        }
        else if (regs_mode == immediate_addr){
            printf("[0] Immediate memory mode\n");
        }
        else if (regs_mode == COMMA){
            printf(" , :(\n");
        }
        else if (regs_mode == STOP){
            printf("Shoudld stop the asm prog\n");
        }
        else if (regs_mode == OTHER){
            printf("[?] External/Relocatable memory mode\n");
        }
    }
    else{
        if (resault == COMMENT){
            ret_val = COMMENT;
        } else if (ret_val != EMPTY){
            printf("Unknown error\n");
        }
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
    return ret_val;
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
    regex_t regex;
    int ret;
    const char* pattern = "^[ \t\n]*\\0?$";
    ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret) {
        regfree(&regex);
        return -1; 
    }
    ret = regexec(&regex, buffer, 0, NULL, 0);
    regfree(&regex);
    if (!ret) {
        return EMPTY;
    } else if (ret == REG_NOMATCH) {
        return OTHER;
    } else {
        return -1;
    }
}

int check_label_type(char * line, const char * main_pattern){
    regex_t rgx;
    int resualt;
    resualt = regcomp(&rgx, main_pattern, REG_EXTENDED);
    if (resualt) {
        regfree(&rgx);
        return -1; 
    }
    resualt = regexec(&rgx, line, 0, NULL, 0);
    regfree(&rgx);
    if (!resualt) {
        return 1;
    } else if (resualt == REG_NOMATCH) {
        return 0;
    } else {
        return -1;
    }
    return resualt;
}

int check_label(char * line){
    regex_t rgx;
    int resualt;
    const char* main_pattern = "^.*[:|.].*$"; // /* insert out special pattern */ = "^[ \t\n]*\\0?$";
    resualt = regcomp(&rgx, main_pattern, REG_EXTENDED);
    if (resualt) {
        regfree(&rgx);
        return -1; 
    }
    resualt = regexec(&rgx, line, 0, NULL, 0);
    regfree(&rgx);
    if (!resualt) {
        return 1;
    } else if (resualt == REG_NOMATCH) {
        return 0;
    } else {
        return -1;
    }
    return resualt;
}