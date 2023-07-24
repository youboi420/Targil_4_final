#include "../include/second_parse.h"
#include "../include/first_parse.h"
#include "../include/utils.h"
#include <error.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>

enum LABEL_TYPE{DEF = 88, USE};

char * instr_rgx[] = {
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "^\\s+rts\\s+$",
    "^\\s+stop\\s+$",
};

char *intr_bin_str[] = {
 "0000000000000000",
 "0000000000000010",
 "0000000000000100",
 "0000000000001000",
 "0000000000010000",
 "0000000000100000",
 "0000000001000000",
 "0000000010000000",
 "0000000100000000",
 "0000001000000000",
"0000010000000000",
"0000100000000000",
"0001000000000000",
"0010000000000000",
"0100000000000000",
"01001000000000000000"
};

int valid_line(char * buffer){
    int ret_val = 1;
    /*  check if the line is not valid in any way -> 
     *  it means. we need to check that for the given instruction the operands are valid.
    */
    return ret_val;
}

int proccess_line(char * line){
    regex_t rgx;
    int resualt;
    const char* main_pattern = "^[ \t\n]*\\0?$";; // /* insert out special pattern */ = "^[ \t\n]*\\0?$";
    resualt = regcomp(&rgx, main_pattern, REG_EXTENDED);
    if (resualt) {
        regfree(&rgx);
        return -1; 
    }
    resualt = regexec(&rgx, line, 0, NULL, 0);
    regfree(&rgx);
    if (!resualt) {
        return PASSED;
    } else if (resualt == REG_NOMATCH) {
        return SYNTAX_EXCEPTION;
    } else {
        return -1;
    }
    return resualt;

}


int parse2_file_stage_1(char * file_name, int *IC, int *DC){
    if (!file_name) {
        printf("[!] Error Invalid file name in parse2 stage2\n");
        exit(1);
    }
    
    FILE *fp = fopen(file_name, "r+");
    char * buffer = (char *)malloc(sizeof(char) * 82), c;
    int counter = 0, line = 0, res, status = 1, label_type, ret_val = PASSED;
    int func_t, mode;

    if (!fp){
        printf("[/] Error opening file in parse2 stage2\n");
        exit(1);
    }

    /*  בדיקה של רגקס על כל שורה 
     *  סינון הקלט לפי סוגו למשל מספר אופרדנים סוגי מיעון וכו
     *  לאחר מכן צריכים לסנן לטבלת הסמלים ולכתוב מילים שמורות בזיכרון התוכנית (כלומר לשריין) אחרי זאת לסגור את הקובץ 
    */
    while(!feof(fp) && status){
        counter = 0;
        /* reads one line in the rules of the question */
        while(counter < 80 && (c = getc(fp)) != '\n'){
            if (c != EOF)
                buffer[counter] = c;
            else{
                buffer[counter] = '\0';
            }
            ++counter;
        }
        buffer[counter] = '\0';
        /* check if the line is valid or not */
        if(counter == 80 && c == '\n'){
            printf("Invalid line length [! Above 80 Chars !] at LINE: %i\n", line);
            fclose(fp);
            return LINE_EXCEPTION;
        }
        mode = get_mode(buffer);
        
        printf("[LINE: %i]\t%s\n", 100 + line,buffer);
        res = valid_line(buffer);
        if (res == 1){
            if (mode == EMPTY)  { printf("[!] Empty line continuing\n"); }
            if (mode == COMMENT){ printf("[!] This is a comment should ignore\n"); }
            res = check_inst(buffer);
            if (res-101 >= 0 && res -101 <= 15)
                printf("MODE: %i SO WORD IS: %s\n", res, intr_bin_str[res-101]);
            // printf("Rs: %i\n", res);
            // /* save helper word's in memory and print current word aka instruction and insert symbol if necessery */
            switch (res) {
                case LABEL:
                    printf("LABEL Should check if usage\\definition\n");
                    /* check if it's defenition or usage of the label */
                    label_type = check_arrtib(buffer);
                    switch (label_type) {
                        case ENT:
                            printf(".Entry \n");
                            break;
                        case EXT:
                            printf(".Extern \n");
                            break;
                        case STR:
                            printf(".String \n");
                            break;
                        case DATA:
                            printf(".Data \n");
                            break;
                        default:
                            printf("Unknown label \n");
                            break;
                    }
                    // get_label_name();
                    break;
                case CMP:
                    // something 0,1,2,3
                    printf("CMP \n");
                    break;
                case PRN:
                    // something 0,1,2,3
                    printf("PRN\n");
                    break;
                case MOV:
                    printf("MOV\n");
                    break;
                case ADD:
                    printf("ADD\n");
                    func_t = 10;
                    break;
                case SUB:
                    printf("SUB\n");
                    func_t = 11;
                    break;
                case LEA:
                    printf("LEA\n");
                    break;
                case CLR:
                    printf("CLR\n");
                    func_t = 10;
                    break;
                case NOT:
                    printf("NOT\n");
                    func_t = 11;
                    break;
                case DEC:
                    printf("DEC\n");
                    func_t = 13;
                    break;
                case INC:
                    printf("INC \n");
                    func_t = 12;
                    break;
                case RED:
                    // something 1,2,3
                    printf("RED\n");
                    break;
                case JMP:
                    printf("JMP\n");
                    func_t = 10;
                    break;
                case BNE:
                    printf("BNE\n");
                    func_t = 11;
                    break;
                case JSR:
                    // something 1,2
                    printf("JSR\n");
                    func_t = 12;
                    break;
                case RTS:
                        // None
                    printf("RTS\n");
                    break;
                case STOP:
                    // None 
                    printf("STOP\n");
                    status = 0;
                    printf("Stopping\n");
                    break;
                default:
                    printf("OTHER\n");
                    
            }
        } else{
            printf("Syntax error at line: %i\n", line);
            status = -1;
        }
        ++line;
        printf("\n");
    }
    if (status == 0) ret_val = STOP;
    if (status == -1) ret_val = SYNTAX_EXCEPTION;
    fclose(fp);
    return ret_val;
}

int parse2_file_stage_2(char * file_name, int *IC, int *DC, int EXPORT_FILES){
    return 1;
}

void read_to_symTable(symPTR node){
    
}

int update_symbole(symPTR *manager, int ic, void * new_data, int data_type){
    return 1;
}

void create_ob_file( symPTR *manager, char * file_name){

}

void create_ent_file( entPTR *manager, char * file_name){

}

void create_ext_file( extPTR *manager, char * file_name){

}