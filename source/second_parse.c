#include "../include/second_parse.h"
#include "../include/first_parse.h"
#include "../include/helper_functions.h"
#include "../include/utils.h"
#include <ctype.h>
#include <error.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>

int mem_mode_info(char *buffer, int *info_arr){
    if (sizeof(info_arr) != sizeof(int) * 2) return -1;
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
        
        if (buffer_to_check[index] == '#') ret_val = info_arr[0] = direct_addr;
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
                    info_arr[0] = direct_addr;
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

        while(isspace(buffer_to_check[index])) ++index;
        ++index;
        while(isspace(buffer_to_check[index])) ++index;
        if (info_arr[0] == 0) info_arr[0] = -1; 
        if (buffer_to_check[index] == '#'){
            flag = skip = 1;
            info_arr[1] = direct_addr;
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
                    info_arr[1] = direct_addr;
                    free(reg_name);
                    break;
                }
                free(reg_name);
            }
            if (strlen(reg) != 0 && i >= 16 && !skip)  info_arr[1] = OTHER;
            if(strlen(reg) != 0 && flag && i < 16) if(ret_val == direct_addr) info_arr[1] = direct_addr;
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
 "0ARE0000000000000000",
 "0ARE0000000000000010",
 "0ARE0000000000000100",
 "0ARE0000000000001000",
 "0ARE0000000000010000",
 "0ARE0000000000100000",
 "0ARE0000000001000000",
 "0ARE0000000010000000",
 "0ARE0000000100000000",
 "0ARE0000001000000000",
"0ARE0000010000000000",
"0ARE0000100000000000",
"0ARE0001000000000000",
"0ARE0010000000000000",
"0ARE0100000000000000",
"01001000000000000000"
};

int valid_line(char * buffer){
    int ret_val = 1;
    /*  check if the line is not valid in any way -> 
     *  it means. we need to check that for the given instruction the operands are valid.
    */
    return ret_val;
}

int calc_delta(int arr[2]){
    int sum = 0;
    /* if function not rts \ stop we need extra func_t word */
    if(arr[0] == direct_addr) sum += 1;
    else if (arr[0] == OTHER) sum += 2;
    if(arr[1] == direct_addr) sum += 1;
    else if (arr[1] == OTHER) sum += 2;
    return sum;
}

char * find_symbol(char * buffer, int *stopped){
    int index = 0;
    *stopped = 0;
    char * name = (char *)malloc(80);

    while(isspace(buffer[index])) ++index;
    if (!((buffer[index] <= 'z' && buffer[index] >= 'a' ) || (buffer[index] <= 'Z' && buffer[index] >= 'A' ))) return NULL;
    
    while(index < 80 && buffer[index]!=':' && buffer[index]){
        name[(*stopped)++] = buffer[index++];
    }
    name[(*stopped)] = '\0';
    *stopped = index;
    return name;
}

int check_arrtib(char *buffer, int start_index){
    int ret_val = OTHER, index = start_index + 1;
    while(isspace(buffer[index])) ++index;

    if(buffer[index] != '.') ret_val = CODE;
    else if (buffer[index] == '.'){
        if(buffer[index+1] == 'e'){
            /* check if ext or ent */
            if(buffer[index+2] == 'x'){ ret_val = EXT; }
            if(buffer[index+2] == 'n'){ ret_val = ENT; }
        }
        else if (buffer[index+1] == 'd'){
            ret_val = DATA; 
        }
        else if (buffer[index+1] == 's'){
            ret_val = DATA; 
        }
        else 
            ret_val = OTHER; 
    }
    return ret_val;
    // if(){

    // }
    
    // if (check_label_type(buffer, "^\\s(.?)(:)\\s+((\\.)entry).*$")) ret_val = ENT;
    // if (check_label_type(buffer, "^\\s(.?)(:)\\s+(\\.extern).*$")) ret_val = EXT;
    // if (check_label_type(buffer, "^\\s(.?)(:)\\s+(\\.data).*$")) ret_val = DATA;
    // if (check_label_type(buffer, "^\\s(.?)(:)\\s+(\\.string).*$")) ret_val = STR;
    printf("\nBuffer: %s\tREG: %i\n", buffer,ret_val);
    // if (ret_val == -1) ret_val = check_label(buffer);
    // if (ret_val == 1) ret_val = OTHER;
    return ret_val;
}

char* handle_opps(symPTR * table_root, char * buffer, int M, FILE *fp){
    switch (M) {
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
        // None
        case RTS:

            break;            
    }
    return NULL;
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

int calc_base_addr(int addr){
    int copy = addr;
    while(copy % 16 != 0 && addr > 0) --copy;
    return copy;
}

int parse2_file_stage_1(char * file_name, int *IC, int *DC, symPTR * root){
    if (!file_name) {
        printf("[!] Error Invalid file name in parse2 stage2\n");
        exit(1);
    }
    
    FILE *fp = fopen(file_name, "r+");
    char *  find_sym;
    char * buffer = (char *)malloc(sizeof(char) * 82), c;
    int counter = 0, line = 0, res, status = 1, label_type, ret_val = PASSED, delta;
    int func_t, mode, arr[2],stopped;

    *root = NULL;
    symPTR node;

    if (!fp){
        printf("[/] Error opening file in parse2 stage2\n");
        exit(1);
    }

    /*  בדיקה של רגקס על כל שורה 
     *  סינון הקלט לפי סוגו למשל מספר אופרדנים סוגי מיעון וכו
     *  לאחר מכן צריכים לסנן לטבלת הסמלים ולכתוב מילים שמורות בזיכרון התוכנית (כלומר לשריין) אחרי זאת לסגור את הקובץ 
    */
    while(!feof(fp) && status){
        stopped = 0;
        node = (symPTR)malloc(sizeof(symNode));
        counter = 0;
        arr[0] = arr[1] = 0;
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
            else if (mode == COMMENT){ printf("[!] This is a comment should ignore\n"); }
            else {    
                res = check_inst(buffer);
                if (res-101 >= 0 && res -101 <= 15){
                    if (mode == reg_addr || mode == direct_addr){
                        printf("0100\t");
                    }
                    else if (mode == immediate_addr || mode == indx_addr) printf("00??\t"); 
                    printf("MODE: %i SO WORD IS: %s", res, intr_bin_str[res-101]);
                    printf("\n\t%s", word_to_ob_line(intr_bin_str[res-101]));
                    mem_mode_info(buffer, arr);
                    delta = calc_delta(arr);
                    if (res != RTS && res != STOP) ++delta; /* for func_t */
                    else if(res == RTS || res == STOP) delta = 1;
                    printf("\nARR[0] = %i\tARR[1] = %i\tDelta: %i\n", arr[0],arr[1], delta);
                }
                // printf("Rs: %i\n", res);
                // /* save helper word's in memory and print current word aka instruction and insert symbol if necessery */
                switch (res) {
                    case LABEL:
                        printf("LABEL Should check if usage\\definition\n");
                        /* check if it's defenition or usage of the label */
                        
                        find_sym = find_symbol(buffer, &stopped);
                        if (find_sym != NULL){
                            printf("symbol found %s\n", find_sym);
                            node->symbol = find_sym;
                            node->baseAddress = calc_base_addr((100 + line));
                            node->offset = (100 + line) - node->baseAddress;
                            node->START_IC = node->value = 100 + line;
                            node->END_IC = delta;
                            for(int a_indx=0; a_indx<N;a_indx++)
                                node->arr[a_indx] = 0;
                            node->next = NULL;
                            insert_symTable(root, node);
                        } else printf("Weird Buffer = %s\n", buffer);
                        
                        label_type = check_arrtib(buffer, stopped);
                        switch (label_type) {
                            case EXT:
                                printf(".Extern \n");
                                break;
                            case ENT:
                                printf(".Entry \n");
                                break;
                            case STR:
                                printf(".String \n");
                                break;
                            case DATA:
                                printf(".Data \n");
                                break;
                            case CODE:
                                printf(".Code\n");
                                break;
                            default:
                                printf("Unknown label %i\n", label_type);
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
                        break;
                    default:
                        printf("OTHER\n");
                        
                }
                for (int l = 0; l < delta; l++){++line;}
                
            }
        } else{
            printf("Syntax error at line: %i\n", line);
            status = -1;
        }
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