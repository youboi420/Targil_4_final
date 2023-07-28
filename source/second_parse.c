#include "../include/second_parse.h"
#include "../include/first_parse.h"
#include "../include/helper_functions.h"
#include "../include/utils.h"
#include <ctype.h>
#include <error.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>

void init_MMN(mem_mode_node *node){
    node->location = -1;
    node->arr[0] = node->arr[1] = 0;
    node->next = NULL; 
}

mem_mode_node * create_MMN(){
    mem_mode_node * new_node = (mem_mode_node *) malloc(sizeof(mem_mode_node));
    init_MMN(new_node);
    return new_node;
}

void insert_mmn(mem_mode_node** root, mem_mode_node *node){
    if(*root==NULL)
        (*root)=node;
    else{
        mem_mode_node* temp = *root;
        while(temp->next)
            temp = temp->next;
        temp->next =  node;
    }
}


void print_mmn_list(mem_mode_node**root){
    mem_mode_node* temp = *root;
    printf("\n------------- MMN LIST ----------------\n");
    while(temp){
        printf("LOC: %i, ARR[0] = %i, ARR[1] = %i\n", temp->location, temp->arr[0], temp->arr[1]);
        temp = temp->next;
    }
    printf("\n------------- END MMN LIST ----------------\n");
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
 "0000000000000000",     /* mov */
 "0000000000000010",     /* ...p */
 "0000000000000100",     /* ...p */
 "0000000000001000",     /* ...p */
 "0000000000010000",     /* ...p */
 "0000000000100000",     /* ...p */
 "0000000001000000",     /* ...p */
 "0000000010000000",     /* ...p */
 "0000000100000000",     /* ...p */
 "0000001000000000",     /* ...p */
"0000010000000000",     /* s... */
"0000100000000000",     /* s... */
"0001000000000000",     /* s... */
"0010000000000000",     /* ... */
"01000100000000000000", /* rts */
"01001000000000000000"  /* stop */
};

int valid_line(char * buffer){
    int ret_val = 1;
    /*  check if the line is not valid in any way -> 
     *  it means. we need to check that for the given instruction the operands are valid.
    */
    return ret_val;
}

/**
 * @brief this function does what we call stage 1 of the second parse it saves all the symbols to the symbole table as required 
 * it makes sure that the assembler is aware of all symbols, 
 * 
 * @param file_name the file name the assembler got 
 * @param IC  the memory addres pointer
 * @param DC  the data addres pointer
 * @param root the symTable manager 
 * @return int returns the function status
 */
int parse2_file_stage_1(char * file_name, int *IC, int *DC, symPTR * root, mem_mode_node** mmn_root){
    if (!file_name) {
        printf("[!] Error Invalid file name in parse2 stage2\n");
        exit(1);
    }
    
    FILE *fp = fopen(file_name, "r+");
    char * buffer = (char *)malloc(sizeof(char) * 82), *  find_sym,  c, *word, sym_name[74] = {'\0'};
    int counter = 0, curr_line = 1,mem_line = 0, res, status = 1, label_type, ret_val = PASSED, delta = 1;
    int func_t, mode, arr[2],stopped, R_E_type = -1, label_index = 0, turn_on, name_len, name_index, found = 0;
    *root = NULL;
    symPTR node, temp;
    mem_mode_node * mmn_node;
    if (!fp){
        printf("[/] Error opening file in parse2 stage2\n");
        exit(1);
    }
    /*  בדיקה של רגקס על כל שורה 
     *  סינון הקלט לפי סוגו למשל מספר אופרדנים סוגי מיעון וכו
     *  לאחר מכן צריכים לסנן לטבלת הסמלים ולכתוב מילים שמורות בזיכרון התוכנית (כלומר לשריין) אחרי זאת לסגור את הקובץ 
    */
    while(!feof(fp) && status){
        turn_on = -1; /* the type of the current label [0,1,2,3] */
        stopped = 0;
        found = 0;
        /* the length of the current insruction in memory */
        delta = 0;
        /* the symbols name */
        sym_name[0] = '\0';
        /* the node to insert to the symTable */
        node = (symPTR)malloc(sizeof(symNode));
        /* the string that holds the current word */
        word = (char *)malloc(sizeof(char) * 21);
        /* num of chars in current line */
        counter = 0;
        /* array of modes */
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
            printf("Invalid line length [! Above 80 Chars !] at LINE: %i\n", mem_line);
            fclose(fp);
            return LINE_EXCEPTION;
        }
        mode = get_mode(buffer);
        printf("[LINE: %i]\t%s\n", 100 + curr_line,buffer);
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
                    mmn_node = create_MMN();
                    mmn_node->arr[0] = arr[0];
                    mmn_node->arr[1] = arr[1];
                    mmn_node->location = curr_line;

                    insert_mmn(mmn_root, mmn_node);
                    print_mmn_list(mmn_root);

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
                            node->baseAddress = calc_base_addr((100 + mem_line));
                            node->offset = (100 + mem_line) - node->baseAddress;
                            node->value = 100 + mem_line;
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
                for (int l = 0; l < delta /* && delta < 5 */; l++){++mem_line;}
            }
        } else{
            printf("Syntax error at line: %i\n", curr_line);
            status = -1;
        }
        ++curr_line;
        printf("\n");
    }
    if (status == 0) ret_val = STOP;
    if (status == -1) ret_val = SYNTAX_EXCEPTION;
    free(word);
    fclose(fp);
    *IC = mem_line;
    return ret_val;
}

/**
 * @brief this function parses the file for the last time before outputing the requested files if the EXPORT_FILES flag is on
 * 
 * @param file_name the file to parse
 * @param IC 
 * @param DC 
 * @param EXPORT_FILES the flag wheter to export or not
 * @return int 
 */
int parse2_file_stage_2(char * file_name, int *IC, int *DC, int EXPORT_FILES, symPTR * root){
    FILE *main_fp, *helper_fp;
    FILE *ob_fp;
    
    symPTR curr = *root;
    entPTR * ent_root;
    extPTR * ext_root;
    entPTR ent_node;
    extPTR ext_node;
    char *buffer, c, *sym_name, *file_name_copy_no_delim = (char *)malloc(strlen(file_name) + 1), *helper_name = (char *)malloc(strlen(file_name) + 1), *num_bin_str, *char_bin_str, current_char; /* current_char is for .string */
    char *file_name_ob = (char *)malloc(strlen(file_name) + 4);
    char *bin_word, *hex_word;
    int symbole_type, i, arr[2] = {0, 0}, counter, instr_flag , comma , opp1 , opp2, line, delta, index, type_dot, index_bkup, minus_flag = 0,current_number; /* current_number is for .data */
    int name_len, is_instr, recover_index;
    
    strcpy(file_name_copy_no_delim, file_name);
    
    if (!file_name) {
        printf("[!] Error Invalid file name in parse2 stage2\n");
        exit(1);
    }
    main_fp = fopen(file_name, "r+");
    if (!main_fp || !ob_fp){
        printf("[/] Error opening file in parse2 stage2\n");
        exit(1);
    }
    ent_root = (entPTR *)malloc(sizeof(entNode));
    ext_root = (extPTR *)malloc(sizeof(entNode));
    ent_node = (entPTR)malloc(sizeof(entNode));
    ext_node = (extPTR)malloc(sizeof(entNode));
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

    /* inorder to get the filename without the [.] for example 1.end [{1 }. end] */
    strtok(file_name_copy_no_delim, ".");
    strcpy(file_name_ob, file_name_copy_no_delim);
    strcat(file_name_ob, ".ob");
    ob_fp = fopen(file_name_ob, "w+");
    
    /* create's the ext and ent files */
    if(file_name_copy_no_delim){
        printToFile_entTable(ent_root, file_name_copy_no_delim);
        printToFile_extTable(ext_root, file_name_copy_no_delim);
    }

    strcpy(helper_name, file_name_copy_no_delim);
    strcat(helper_name, ".help");
    helper_fp = fopen(helper_name, "w");
    
    line = 100;
    buffer = (char *)malloc(sizeof(char) * 81);
    while(!feof(main_fp)){
        delta = counter = instr_flag = comma = opp1 = opp2 = 0;
        arr[0] = arr[1] = 0;
        fgets(buffer, 81, main_fp);
        /* in the start of the label/code */
        index = trim(buffer);
        if(buffer[index] == '.'){
            // x: prn #
            type_dot = check_arrtib(buffer, index);
            handle_data(type_dot, helper_fp, buffer, index, root);
        }
        else{
            is_instr = check_inst(buffer);
                if (is_instr-(LABEL+1) >= 0 && is_instr - (LABEL+1) <= 15){
                    // WORD
                    // 19 18 17 ........ 2 1 0
                    // mov L, r1
                    // 0ARE | funct 0000 | SOURCE 0000 | ADDR_MODE 00 | DEST 0000 | ADDR_MODE 00 |
                    // all instr are 0100
                    // help words is : 0??? according to mmn_node at location [curr line]
                    // if () {
                    // }
                    // strcpy(bin_word, "0100")
                    // check_opp1()
                    // strcat -> not sure? [num/ reg / extern / entry]
                    // strcat(bin_word, to_bin(func_t))
                    bin_word = (char*)malloc(sizeof(char) * 21);
                    strcpy(bin_word, "0100");
                    index += 3; /* after the instr exluding stop */
                    /* mov #1,r1 */
                    
                    switch (is_instr) {
                            case CMP:  /* 2 */ 
                                recover_index = index;
                                opp1 = OTHER;
                                printf("BF: %s\n", &buffer[index]);
                                opp1 = get_opp_type(buffer, &index);
                                switch (opp1) {
                                    case type_num:
                                        printf("number\n");
                                        break;
                                    case type_reg:
                                        printf("reg\n");
                                        break;
                                    case OTHER:
                                        printf("need to check\n");
                                        /* check if ent or ext */
                                        break;
                                    default:
                                        printf("TYPE\n");
                                        break;
                                        
                                }
                                printf("LEFTOVER: %s\n", &buffer[index]);
                                opp2 = OTHER;
                                opp2 = get_opp_type(buffer, &index);
                                switch (opp2) {
                                    case type_num:
                                        break;
                                    case type_reg:
                                        break;
                                    case OTHER:
                                        /* check if ent or ext */
                                        break;
                                    default:
                                        printf("TYPE\n");
                                        break;
                                }
                                break;
                            case PRN:  /* 1 */
                                break;
                            case MOV:  /* 2 */
                                break;
                            case ADD:  /* 2 */
                                break;
                            case SUB:  /* 2 */
                                break;
                            case LEA:  /* 2 */
                                break;
                            case CLR:  /* 1 */
                                break;
                            case NOT:  /* 1 */
                                break;
                            case DEC:  /* 1 */
                                break;
                            case INC:  /* 1 */
                                break;
                            case RED:  /* 1 */
                                break;
                            case JMP:  /* 1 */
                                break;
                            case BNE:  /* 1 */
                                break;
                            case JSR:  /* 1 */
                                break;
                            case RTS:  /* 0 */
                                strcpy(bin_word, intr_bin_str[14]);
                                break;
                            case OTHER:
                                break;
                            default:
                                break;
                    } 
                } else { /* it's a label */

                }
        }
        
        printf("[LINE: %i] %s", line, buffer);         
        ++line;
        buffer[0] = '\0';
        /* check if the line is valid or not */
    }
    // mem_mode_info(buffer, arr);
    // delta = calc_delta(arr);
    fclose(main_fp);
    fclose(helper_fp);
    free(buffer);
    return 1;
}

int get_opp_type(char * buffer, int *indx){
    int ret_val = OTHER, *trash;
    if (buffer[*indx] == '\0') return OTHER;
    while(isspace(buffer[*indx])) ++(*indx);
    if(buffer[*indx] == '#') ret_val = type_num;
    else if (buffer[*indx] == 'r' && buffer[(*indx) + 1] != '\0'){
        if (check_regs(&buffer[(*indx) + 1], trash)) ret_val = type_reg;
        else ret_val = OTHER;
    }
    while(buffer[*indx] != '\0' && !isspace(buffer[*indx]) && buffer[*indx] != ',') ++(*indx);
    ++(*indx);
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

void handle_data(int type_dot, FILE * helper_fp, char * buffer, int index, symPTR *root){
    int index_bkup, name_len, current_number, minus_flag = 0;
    char *sym_name, current_char, *char_bin_str, *num_bin_str, *line, *hex_word;
    line = (char *)malloc(sizeof(char) * 21);
    switch (type_dot) {
                case EXT:
                    // printf("type: EXT, %s\n", &buffer[index]);
                    /* put zero inside the symTable if you find the label */
                    index += 7;
                    index_bkup = index;
                    while(isspace(buffer[index])) ++index;
                    sym_name = (char *)malloc(74); /* the size of the string without .extern */
                    strcpy(sym_name, &buffer[index]);
                    name_len = 0;
                    while(!isspace(buffer[index])) {
                        ++name_len;
                        ++index;
                    }
                    sym_name[name_len] = '\0';
                    index = index_bkup;
                    ext_put_zero(sym_name, root);
                    break;
                case STR:
                    index += 7; 
                    while(buffer[index]){
                        if( ! isspace(buffer[index] ) ){
                            if (buffer[index] != '"'){
                                current_char = buffer[index];
                                strcpy(line, "0100");
                                strcat(line,  num_to_bin_str((int)current_char));
                                hex_word = word_to_ob_line(line);
                                fputs(hex_word, helper_fp);
                                fputs("\n", helper_fp);
                            }
                        }
                        ++index;
                    }
                    // 2 .string "ancndvncdv" -> helper file
                    break;
                case DATA:
                    index += 5;
                    // printf("type: DATA, buffer = %s\n", &buffer[index]);
                    // 1 .data 1,2            ,2         ,2, -2,2,2,2,2,2 ->  helper file
                    current_number = INT_MIN;
                    while(buffer[index]){
                        if( ! isspace(buffer[index] ) ){
                            if (buffer[index] == '-') minus_flag = 1;
                            else if (buffer[index] == '+') minus_flag = 0;
                            else if(current_number == INT_MIN){
                                current_number = (int)(buffer[index] - '0');
                            }
                            else {
                                if (buffer[index] == ','){
                                    if (minus_flag) current_number *= -1;
                                    num_bin_str = num_to_bin_str(current_number);
                                    strcpy(line, "0100");
                                    strcat(line, num_bin_str);
                                    hex_word = word_to_ob_line(line);
                                    fputs(hex_word, helper_fp);
                                    fputs("\n", helper_fp);
                                    current_number = INT_MIN;
                                    
                                }
                                else{
                                    current_number = current_number * 10 + (int)(buffer[index] - '0');
                                }
                            }
                        }
                        ++index;
                    }
                    num_bin_str = num_to_bin_str(current_number);
                    strcpy(line, "0100");
                    strcat(line, num_bin_str);
                    hex_word = word_to_ob_line(line);
                    fputs(hex_word, helper_fp);
                    fputs("\n", helper_fp);
                    break;
                case OTHER:
                    printf("type: UNKOWN\n");
                    break;
            }
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
        
        if (buffer_to_check[index] == '#') info_arr[0] = direct_addr;
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
        if (info_arr[0] == 0 && (buffer_to_check[index] != '\n' || buffer_to_check[index] != '\0')) info_arr[0] = -1; 
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

void ext_put_zero(char * node_name, symPTR *root){
    symPTR temp = *root;
    while(temp){
        if (strcmp(node_name, temp->symbol) == 0){
            /* for safety */
            if (temp->arr[0] == 1){
                temp->baseAddress = 0;
                temp->value = 0;
                temp->offset = 0;
                temp->END_IC = 0;
                temp->START_IC = 0;
            }
        }
        temp = temp->next;
    }
}

int calc_base_addr(int addr){
    int copy = addr;
    while(copy % 16 != 0 && addr > 0) --copy;
    return copy;
}

char* num_to_bin_str(int number) {
    char *bits = malloc(sizeof(char) * 17);
    unsigned int un_number = (unsigned int)number;
    strcpy(bits, "0000000000000000");
    int bits_index = 15, flag = 1;
    while ( un_number > 0 ) {
        bits[bits_index--] = (un_number & 1) + '0';
        un_number = ( un_number >> 1);
    }
    bits[16] = '\0';
    return bits;
}