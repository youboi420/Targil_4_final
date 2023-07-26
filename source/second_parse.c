#include "../include/second_parse.h"
#include "../include/first_parse.h"
#include "../include/helper_functions.h"
#include "../include/utils.h"
#include <ctype.h>
#include <error.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>

int check_exists(char * name_to_search, symPTR * root){
    symPTR temp = *root;
    int ret_val = 0;
    while(temp){
        if(strcmp(name_to_search, temp->symbol) == 0){
                ret_val = 1;
                temp = NULL;
                break;
        }
        temp = temp->next;
    }
    return ret_val;
}

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
"01000100000000000000",
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

/**
 * @brief this function checks for symbol's/variable name inside the given buffer and inc's the given stopped cariable pointer 
 * 
 * @param buffer the string to look for a label's name inside
 * @param stopped  the pointer to the stopped index
 * @return char* 
 */

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
    int ret_val = OTHER, index = start_index;
    while(isspace(buffer[index])) ++index;
    if(buffer[index] != '.'){
        ret_val = CODE;
    }
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
            ret_val = STR; 
        }
        else 
            ret_val = OTHER; 
    }
    return ret_val;
    printf("\nBuffer: %s\tREG: %i\n", buffer,ret_val);
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
    char * buffer = (char *)malloc(sizeof(char) * 82), *copy_buffer, *  find_sym,  c, *word, sym_name[74] = {'\0'};
    int counter = 0, line = 0, res, status = 1, label_type, ret_val = PASSED, delta = 1;
    int func_t, mode, arr[2],stopped, R_E_type = -1, label_index = 0, turn_on, name_len, name_index, found = 0;
    *root = NULL;
    symPTR node, temp;

    if (!fp){
        printf("[/] Error opening file in parse2 stage2\n");
        exit(1);
    }

    /*  בדיקה של רגקס על כל שורה 
     *  סינון הקלט לפי סוגו למשל מספר אופרדנים סוגי מיעון וכו
     *  לאחר מכן צריכים לסנן לטבלת הסמלים ולכתוב מילים שמורות בזיכרון התוכנית (כלומר לשריין) אחרי זאת לסגור את הקובץ 
    */
    while(!feof(fp) && status){
        turn_on = -1;
        stopped = 0;
        found = 0;
        delta = 0;
        sym_name[0] = '\0';
        node = (symPTR)malloc(sizeof(symNode));
        word = (char *)malloc(sizeof(char) * 21);
        copy_buffer = (char *)malloc(sizeof(char) * 21);
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
                
                /* make the calculation to see if the resualt is in the range of the specfied instructions
                *  excluding the labels, hence the (LABEL+1) if the resualt is a label well check if it's a code label or not in the second stage
                */
                if (res-(LABEL+1) >= 0 && res - (LABEL+1) <= 15){
                    if (mode == reg_addr || mode == direct_addr){
                        strcpy(word, "0100");
                    }
                    else /* if (mode == immediate_addr || mode == indx_addr) */{
                        strcpy(word, "00??");
                    }
                    if (res == STOP || res == RTS) strcpy(word, "");
                    strcat(word,intr_bin_str[res-101]);
                    
                    printf("MODE: %i SO WORD IS: %s", res, word);
                    printf("\n\t%s", word_to_ob_line(word));
                    
                    mem_mode_info(buffer, arr);
                    delta = calc_delta(arr);
                    
                    if (res != RTS && res != STOP) ++delta; /* for func_t */
                    else if(res == RTS || res == STOP) delta = 1;
                    printf("\nARR[0] = %i\tARR[1] = %i\tDelta: %i\n", arr[0], arr[1], delta);
                }
                /* save helper word's in memory and print current word aka instruction and insert symbol if necessery */
                switch (res) {
                    case LABEL:
                        for(int a_indx=0; a_indx<N;a_indx++){
                            node->arr[a_indx] = 0;
                        }
                        printf("LABEL Should check if usage\\definition\n");
                        /* check if it's defenition or usage of the label */
                        find_sym = find_symbol(buffer, &stopped);
                        label_type = check_arrtib(buffer, stopped);
                        printf("label type: %i\n> %s \n", label_type == STR,&buffer[stopped+1]);
                        
                        mem_mode_info(&buffer[stopped + 1], arr);                
                        delta = calc_delta(arr);
                        /* if the start is .ent or .ext */
                        if (find_sym == NULL){
                            if(buffer[stopped+1] == 'e'){
                                /* check ext pr ent */
                                if (buffer[stopped+2] == 'x'){
                                    /* extern */
                                    turn_on = 0;
                                }
                                else if (buffer[name_index+2] == 'n'){
                                    /* entry */
                                    turn_on = 1;
                                }
                                name_index = stopped;
                                stopped += 7;
                                while(isspace(buffer[stopped])) ++stopped;
                                strcpy(sym_name, &buffer[stopped]);
                                name_len = 0;
                                while(!isspace(buffer[stopped])) {
                                    ++name_index;
                                    ++stopped;
                                }
                                sym_name[name_index] = '\0';
                                stopped = name_index;
                                
                                temp = *root;
                                while(temp){
                                    if(strcmp(sym_name, temp->symbol) == 0 && (turn_on == 1 || turn_on == 0)){
                                        printf("Turned on\n");
                                        temp->arr[turn_on] = 1;
                                        temp = NULL;
                                        break;
                                    }
                                    temp = temp->next;
                                }                   
                            }
                            if (sym_name[0] != '\0'){
                                    printf("Name to find = |%s|\n", sym_name);
                                    R_E_type = check_R_E(buffer, &label_index);
                                    while(isspace(buffer[label_index])) ++label_index;
                                    printf("[^] R:%i E:%i\tfor label: %s\n", R_E_type == ENT , R_E_type == EXT ,&buffer[label_index]);
                                    if (R_E_type == EXT) node->arr[0] = 1;
                                    if (R_E_type == ENT) node->arr[1] = 1;
                            }
                        }
                        else if (buffer[stopped+1] == 'd' && buffer[stopped+2] == 'a' && buffer[stopped+3] == 't' && buffer[stopped+4] == 'a' && buffer[stopped+5] == ' '){ printf("Shoudld handle the given data in memory next round??\n"); }
                        else if (buffer[stopped+1] == 's' && buffer[stopped+2] == 't' && buffer[stopped+3] == 'r' && buffer[stopped+4] == 'i' && buffer[stopped+5] == 'g' && buffer[stopped+6] == ' '){ printf("Shoudld handle the given string in memory next round??\n"); }
                        else{
                            printf("ARR[0] = %i\tARR[1] = %i\tDelta: %i\n", arr[0], arr[1], delta);
                            printf("[?!] Unknown type of info\n");
                        }
                        ++stopped;
                        switch (label_type) {
                            case EXT:
                                printf(".Extern \n");
                                node->arr[0] = 1;
                                break;
                            case ENT:
                                printf(".Entry \n");
                                node->arr[1] = 1;
                                break;
                            case STR:
                                printf(".String \n");
                                node->arr[2] = 1;
                                delta = 0;
                                while((buffer[stopped]) != '"'){ 
                                    ++stopped;
                                }
                                ++stopped;
                                while((buffer[stopped]) != '"'){ 
                                    ++delta;
                                    ++stopped;
                                }
                                printf("Changed delta: %i\n", delta);
                                (*DC) = (*DC) + delta;
                                break;
                            case DATA:
                                printf(".Data \n");
                                node->arr[2] = 1;
                                delta = 1;
                                printf("Changed delta\n");
                                while((buffer[stopped])){ 
                                    if (buffer[stopped] == ',') ++delta;
                                    ++stopped;
                                }
                                // delta = ?;
                                (*DC) = (*DC) + delta;
                                break;
                            case CODE:
                                printf(".Code\n");
                                node->arr[3] = 1;
                                break;
                            default:
                                printf("Unknown label %i\n", label_type);
                                break;
                        }
                        if (find_sym != NULL){
                            printf("symbol found %s\n", find_sym);
                            node->symbol = find_sym;
                            node->baseAddress = calc_base_addr((100 + line));
                            node->offset = (100 + line) - node->baseAddress;
                            node->value = 100 + line;
                            node->END_IC = delta;
                            node->next = NULL;
                            if(! check_exists(find_sym, root)){
                                insert_symTable(root, node);
                            } else printf("IGNORE\n");
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
                        if (arr[0] == 11){
                            printf("0100 REG/NUMBER MODE\n");
                        } else if (arr[0] == -1) printf("EXTERN/RELOCATABLE\n");
                        else {
                            printf("ARR[0] = %i\n", arr[0]);
                        }
                        break;
                    case INC:
                        printf("INC ?? \n");
                        func_t = 12;
                        if (arr[0] == 11){
                            printf("REG/NUMBER MODE\n");
                        } else if (arr[0] == -1) printf("EXTERN/RELOCATABLE\n");
                        else {
                            printf("ARR[0] = %i\n", arr[0]);
                        }
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
                for (int l = 0; l < delta /* && delta < 5 */; l++){++line;}
                
            }
        } else{
            printf("Syntax error at line: %i\n", line);
            status = -1;
        }
        printf("\n");
    }
    if (status == 0) ret_val = STOP;
    if (status == -1) ret_val = SYNTAX_EXCEPTION;
    free(word);
    fclose(fp);
    *IC = line;
    return ret_val;
}

int check_R_E(char * buffer, int *label_index){
    int index = 0, ret_val = OTHER;
    char STR[8] = {0};
    *label_index = 0;
    
    while(isspace(buffer[index])) ++index;
    while(!isspace(buffer[index]) && *label_index < 7){
        STR[*label_index] = buffer[index];
        ++index;
        ++(*label_index);
    }
    STR[*label_index] = '\0';
    printf("STR: %s\n", STR);
    if (strcmp(STR, ".entry") == 0) ret_val = ENT;
    if (strcmp(STR, ".extern") == 0) ret_val = EXT;
    if (strcmp(STR, ".data") == 0) ret_val = DATA;
    if (strcmp(STR, ".string") == 0) ret_val = DATA;
    return ret_val;
}

/**
 * @brief this function parses the file for the last time before outputing the requested files if the EXPORT_FILES flag is on
 * 
 * @param file_name 
 * @param IC 
 * @param DC 
 * @param EXPORT_FILES 
 * @return int 
 */
int parse2_file_stage_2(char * file_name, int *IC, int *DC, int EXPORT_FILES, symPTR * root){
    symPTR curr = *root;
    entPTR * ent_root = (entPTR *)malloc(sizeof(entNode));
    extPTR * ext_root = (extPTR *)malloc(sizeof(entNode));
    entPTR ent_node =(entPTR)malloc(sizeof(entNode));
    extPTR ext_node =(extPTR)malloc(sizeof(entNode));
    char *buffer, c;
    int symbole_type, i, arr[2], counter, instr_flag , comma , opp1 , opp2;

    FILE *fp = fopen(file_name, "r+");

    if (!fp){
        printf("[/] Error opening file in parse2 stage2\n");
        exit(1);
    }
    
    /* initilize the root's */
    *ent_root = NULL;
    *ext_root = NULL;

    while( curr ){
        /* check's only ext and ent */
        symbole_type = OTHER;
        if (curr->arr[0] == 1) symbole_type = EXT;
        if (curr->arr[1] == 1) symbole_type = ENT;
        switch (symbole_type) {
            case EXT:
                ext_node = create_extTable_node();
                ext_node->symbol = curr->symbol;
                ext_node->baseAddress = (curr->value) - 1;
                ext_node->offset = (curr->value);
                insert_extTable(ext_root, ext_node);
                break;
            case ENT:
                ent_node = create_entTable_node();
                ent_node->symbol = curr->symbol;
                ent_node->baseAddress = curr->baseAddress;
                ent_node->offset = curr->offset;
                insert_entTable(ent_root, ent_node);
                break;
            default:
                break;
        }
        curr = curr->next;
    }


    while(!feof(fp)){
        // mov R, r1
        // 0ARE
        // 0010
        // dec r4

        // stop rts prn inc 
        
        //      inc r1
        //      mov
        //      stop
        
        // counter = instr_flag = comma = opp1 = opp2 = 0;
        // buffer = (char *)malloc(sizeof(char) * 80);
        // while(counter < 80 && (c = getc(fp)) != '\n'){
        //     if (c != EOF){
        //         buffer[counter] = c;
        //         if(c != ' '){
        //             if(instr_flag != 3)
        //                 ++instr_flag;
        //         }
        //     }
        //     else{
        //         buffer[counter] = '\0';
        //     }
        //     ++counter;
        // }
        // buffer[counter] = '\0';
        // /* check if the line is valid or not */
        // free(buffer);
    }
    // mem_mode_info(buffer, arr);
    // delta = calc_delta(arr);
    
    return 1;
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