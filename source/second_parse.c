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
    node->opp_mode[0] = node->opp_mode[1] = 0;
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

mem_mode_node* get_node_by_location(mem_mode_node**  root, int loc){
    mem_mode_node *temp = *root;
    while(temp){
        if (temp->location == loc) return temp;
        temp = temp->next;
    }
    return NULL;
}

void print_mmn_list(mem_mode_node**root){
    mem_mode_node* temp = *root;
    printf("\n------------- MMN LIST ----------------\n");
    while(temp){
        printf("LOC: %i, ARR[0] = %i, ARR[1] = %i\n", temp->location, temp->opp_mode[0], temp->opp_mode[1]);
        temp = temp->next;
    }
    printf("\n------------- END MMN LIST ----------------\n");
}

char *funct_str[] = {
    "1010",
    "1011",
    "1100",
    "1101"
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

int get_are_mode(int *arr){
    int res = OTHER;
    if (sizeof(arr) != sizeof(int) * 2) return res;

    res = arr[0];
    switch (arr[1]) {
        case num_addr:
            if (res != OTHER) res = direct_addr;
            break;
            
        case direct_addr:
            if (res != OTHER) res = direct_addr;
            break;
            
        case OTHER:
            if (res != OTHER) res = OTHER;        
            break;
            
    }
    return res;
}

int trimNotSpace(char * str_to_cut){
    int len = strlen(str_to_cut), i = 0;
    while(i < len && !isspace(str_to_cut[i])) ++i;
    return i;
}

void wrongString(char *input_string){
    regex_t regex;
    int index;
    char *curr = input_string;
    bool ret;
    char *oneOp = ".*,.*";
    char *ThreeOp = ".*,.*,.*";
    char *label = ".*[A-Za-z_][A-Za-z0-9_]*:.*";// check if there's label
    char *namePatt = ".*((stop)|(rts)|(clr)|(not)|(inc)|(dec)|(red)|(jmp)|(bne)|(jsr)|(prn)|(lea)|(sub)|(add)|(mov)|(cmp)|(macro)|(endm)|[.](data)|[.](string)|[.](extern)|[.](entry)).*";
    char *commaOneOp = ".*((stop)|(rts)|(clr)|(not)|(inc)|(dec)|(red)|(jmp)|(bne)|(jsr)|(prn)).*";


    ret = regcomp(&regex, label, REG_EXTENDED | REG_ICASE);
    if (ret) {
        fprintf(stderr, "Could not compile regex\n");
        return;
    }
    ret = regexec(&regex, curr, 0, NULL, 0);
    if(ret==0) {
        index = trim(curr);
        curr+=index;
        index = trimNotSpace(curr);
        curr+=index;
        index = trim(curr);
        curr+=index;
    }
    regfree(&regex);

    ret = regcomp(&regex, namePatt, REG_EXTENDED | REG_ICASE);
    if (ret) {
        fprintf(stderr, "Could not compile regex\n");
        return;
    }
    ret = regexec(&regex, curr, 0, NULL, 0);
    if(ret==1) {
        printf("invalid command\n");
        return;
    }
    regfree(&regex);

    ret = regcomp(&regex, commaOneOp, REG_EXTENDED | REG_ICASE);
    if (ret) {

        fprintf(stderr, "Could not compile regex\n");
        return;
    }
    ret = regexec(&regex, curr, 0, NULL, 0);
    index = trim(curr);
    curr+=index;
    index = trimNotSpace(curr);
    curr+=index;
    index = trim(curr);
    curr+=index;
    if(ret==0) { //if 0/1 operand
        regfree(&regex);
        ret = regcomp(&regex, oneOp, REG_EXTENDED | REG_ICASE);
        if (ret) {
            fprintf(stderr, "Could not compile regex\n");
            return;
        }
        ret = regexec(&regex, curr, 0, NULL, 0);
        if(ret==0) {
            printf("too much operands\n");
            regfree(&regex);
            return;
        }
    }
    else //if 2 operand
    {
        regfree(&regex);
        ret = regcomp(&regex, ThreeOp, REG_EXTENDED | REG_ICASE);
        if (ret) {
            fprintf(stderr, "Could not compile regex\n");
            return;
        }
        ret = regexec(&regex, curr, 0, NULL, 0);
        if(ret==0) {
            printf("too much operands\n");
            regfree(&regex);
            return;
        }
        ret = regcomp(&regex, oneOp, REG_EXTENDED | REG_ICASE);
        if (ret) {
            fprintf(stderr, "Could not compile regex\n");
            return;
        }
        ret = regexec(&regex, curr, 0, NULL, 0);
        if(ret==0) {
            printf("less operands\n");
            regfree(&regex);
            return;
        }
    }
}

void handle_number(FILE *ob_file, char *number_str, int *mem_addr){
    char *bin_word = (char *)malloc(sizeof(char) * 21);
    if (!bin_word){
        printf("Error in malloc handle\n");
        return;
    }
    if ( number_str[0] == '1' ){
        strcpy(bin_word, "0100111111");
    }
    else{
        strcpy(bin_word, "0100000000");
        strcat(bin_word, number_str);
        strcat(bin_word, "110000");
    }

    fprintf(ob_file, "[%4i] ", (*mem_addr)++);
    fputs(word_to_ob_line(bin_word), ob_file);
    fputs("\n", ob_file);

    free(bin_word);
}

void handle_register(FILE *ob_file, char *reg_str, int *mem_addr){
    char *bin_word = (char *)malloc(sizeof(char) * 21);
    if (!bin_word){
        printf("Error in malloc handle\n");
        return;
    }
    
    strcpy(bin_word, "0100000000");
    strcat(bin_word, reg_str);
    strcat(bin_word, "110000");

    fprintf(ob_file, "[%4i] ", (*mem_addr)++);
    fputs(word_to_ob_line(bin_word), ob_file);
    fputs("\n", ob_file);

    free(bin_word);
}

void get_reg_str(int mode, int recover_index, char * buffer, char * reg_str){
    int local_index = 0;
    if (mode == 1){
        while(buffer[recover_index + local_index + 1] != '\0' && buffer[recover_index + local_index + 1] != ',' && buffer[recover_index + local_index + 1] != ' ' && buffer[recover_index + local_index + 1] != '\n'){
            reg_str[local_index] = buffer[recover_index + local_index + 1];
            ++local_index;
        }
    } else if (mode == 2) {
        
        while(buffer[recover_index + local_index + 1] != '\0' && buffer[recover_index + local_index + 1] != ' ' && buffer[recover_index + local_index + 1] != '\n'){
            reg_str[local_index] = buffer[recover_index + local_index + 1];
            ++local_index;
        }
    }
    reg_str[local_index] = '\0';

    if (strcmp(reg_str, "0") == 0 ) {    
        strcpy(reg_str, "0000");
    }

    else if (strcmp(reg_str, "1") == 0 ) {   
        strcpy(reg_str, "0001");
    }

    else if (strcmp(reg_str, "2") == 0 ) {   
        strcpy(reg_str, "0010");
    }

    else if (strcmp(reg_str, "3") == 0 ) {   
        strcpy(reg_str, "0011");
    }

    else if (strcmp(reg_str, "4") == 0 ) {   
        strcpy(reg_str, "0100");
    }

    else if (strcmp(reg_str, "5") == 0 ) {   
        strcpy(reg_str, "0101");
    }

    else if (strcmp(reg_str, "6") == 0 ) {   
        strcpy(reg_str, "0110");
    }

    else if (strcmp(reg_str, "7") == 0 ) {   
        strcpy(reg_str, "0111");
    }

    else if (strcmp(reg_str, "8") == 0 ) {   
        strcpy(reg_str, "1000");
    }

    else if (strcmp(reg_str, "9") == 0 ) {   
        strcpy(reg_str, "1001");
    }

    else if (strcmp(reg_str, "10") == 0 ) {   
        strcpy(reg_str, "1010");
    }

    else if (strcmp(reg_str, "11") == 0 ) {   
        strcpy(reg_str, "1011");
    }

    else if (strcmp(reg_str, "12") == 0 ) {   
        strcpy(reg_str, "1100");
    }

    else if (strcmp(reg_str, "13") == 0 ) {   
        strcpy(reg_str, "1101");
    }

    else if (strcmp(reg_str, "14") == 0 ) {   
        strcpy(reg_str, "1110");
    }

    else if (strcmp(reg_str, "15") == 0 ) {   
        strcpy(reg_str, "1111");
    }
}

void get_num_str(int mode, int recover_index, char *buffer, char *number_str){
    int local_index = 0;
    if (mode == 1){
        while(buffer[recover_index + local_index + 1] != '\0' && buffer[recover_index + local_index + 1] != ',' && buffer[recover_index + local_index + 1] != ' ' && buffer[recover_index + local_index + 1] != '\n'){
            number_str[local_index] = buffer[recover_index + local_index + 1];
            ++local_index;
        }
    } else if (mode == 2) {
        while(buffer[recover_index + local_index + 1] != '\0' && buffer[recover_index + local_index + 1] != ' ' && buffer[recover_index + local_index + 1] != '\n'){
            number_str[local_index] = buffer[recover_index + local_index + 1];
            ++local_index;
        }
    }
    number_str[local_index] = '\0';
    /* positive number */
    if (strcmp(number_str, "0") == 0 || strcmp(number_str, "+0") == 0 || strcmp(number_str, "-0") == 0){
        strcpy(number_str, "0000");
    }
    else if( strcmp(number_str, "1") == 0 || strcmp(number_str, "+1") == 0){
        strcpy(number_str, "0001");
    }
    else if( strcmp(number_str, "2") == 0 || strcmp(number_str, "+2") == 0){
        strcpy(number_str, "0010");
    }
    else if( strcmp(number_str, "3") == 0 || strcmp(number_str, "+3") == 0){
        strcpy(number_str, "0011");
    }
    else if( strcmp(number_str, "4") == 0 || strcmp(number_str, "+4") == 0){
        strcpy(number_str, "0100");
    }
    else if( strcmp(number_str, "5") == 0 || strcmp(number_str, "+5") == 0){
        strcpy(number_str, "0101");
        
    }
    else if( strcmp(number_str, "6") == 0 || strcmp(number_str, "+6") == 0){
        strcpy(number_str, "0110");
    }
    else if( strcmp(number_str, "7") == 0 || strcmp(number_str, "+7") == 0){
        strcpy(number_str, "0111");
    }
    /* negative number */
    else if( strcmp(number_str, "-1") == 0 ){
        strcpy(number_str, "1111");
    }
    else if( strcmp(number_str, "-2") == 0 ){
        strcpy(number_str, "1110");
    }
    else if( strcmp(number_str, "-3") == 0 ){
        strcpy(number_str, "1101");

    }
    else if( strcmp(number_str, "-4") == 0 ){
        strcpy(number_str, "1100");
    }
    else if( strcmp(number_str, "-5") == 0 ){
        strcpy(number_str, "1011");
        
    }
    else if( strcmp(number_str, "-6") == 0 ){
        strcpy(number_str, "1010");
    }
    else if( strcmp(number_str, "-7") == 0 ){
        strcpy(number_str, "1001");
    }    
    else if( strcmp(number_str, "-8") == 0 ){
        strcpy(number_str, "1000");
    }
}



int valid_line(char * input_string){
    int ret_val = 1;
    regex_t regex;
    bool ret;

    char *pattStop = "^\\s*(([A-Za-z_][A-Za-z0-9_]+[:])|([A-Za-z_][:]))?\\s*(stop)\\s*$";
    char *pattRts = "^\\s*(([A-Za-z_][A-Za-z0-9_]+[:])|([A-Za-z_][:]))?\\s*(rts)\\s*$";
    ///1,2,3
    char *pattClr = "^\\s*(([A-Za-z_][A-Za-z0-9_]+[:])|([A-Za-z_][:]))?\\s*(clr)\\s*(([A-Za-z_][A-Za-z0-9_]+\\[r([1-9]|1[0-5])\\])|([A-Za-z_][A-Za-z0-9_]*)|r([1-9]|1[0-5]))\\s*$";
    char *pattNot = "^\\s*(([A-Za-z_][A-Za-z0-9_]+[:])|([A-Za-z_][:]))?\\s*(not)\\s*(([A-Za-z_][A-Za-z0-9_]+\\[r([1-9]|1[0-5])\\])|([A-Za-z_][A-Za-z0-9_]*)|r([1-9]|1[0-5]))\\s*$";
    char *pattInc = "^\\s*(([A-Za-z_][A-Za-z0-9_]+[:])|([A-Za-z_][:]))?\\s*(inc)\\s*(([A-Za-z_][A-Za-z0-9_]+\\[r([1-9]|1[0-5])\\])|([A-Za-z_][A-Za-z0-9_]*)|r([1-9]|1[0-5]))\\s*$";
    char *pattDec = "^\\s*(([A-Za-z_][A-Za-z0-9_]+[:])|([A-Za-z_][:]))?\\s*(dec)\\s*(([A-Za-z_][A-Za-z0-9_]+\\[r([1-9]|1[0-5])\\])|([A-Za-z_][A-Za-z0-9_]*)|r([1-9]|1[0-5]))\\s*$";
    char *pattRed = "^\\s*(([A-Za-z_][A-Za-z0-9_]+[:])|([A-Za-z_][:]))?\\s*(red)\\s*(([A-Za-z_][A-Za-z0-9_]+\\[r([1-9]|1[0-5])\\])|([A-Za-z_][A-Za-z0-9_]*)|r([1-9]|1[0-5]))\\s*$";
    ///1,2
    char *pattJmp = "^\\s*(([A-Za-z_][A-Za-z0-9_]+[:])|([A-Za-z_][:]))?\\s*(jmp)\\s*((([A-Za-z_][A-Za-z0-9_]+\\[r([1-9]|1[0-5])\\])|([A-Za-z_][A-Za-z0-9_]*)))\\s*$";
    char *pattBne = "^\\s*(([A-Za-z_][A-Za-z0-9_]+[:])|([A-Za-z_][:]))?\\s*(bne)\\s*((([A-Za-z_][A-Za-z0-9_]+\\[r([1-9]|1[0-5])\\])|([A-Za-z_][A-Za-z0-9_]*)))\\s*$";
    char *pattJsr = "^\\s*(([A-Za-z_][A-Za-z0-9_]+[:])|([A-Za-z_][:]))?\\s*(jsr)\\s*((([A-Za-z_][A-Za-z0-9_]+\\[r([1-9]|1[0-5])\\])|([A-Za-z_][A-Za-z0-9_]*)))\\s*$";
    ////0,1,2,3
    char *pattPrn = "^\\s*(([A-Za-z_][A-Za-z0-9_]+[:])|([A-Za-z_][:]))?\\s*(prn)\\s*((([A-Za-z_][A-Za-z0-9_]+\\[r([1-9]|1[0-5])\\])|([A-Za-z_][A-Za-z0-9_]*))|r([1-9]|1[0-5])|(#-?[1-8]|#[0-7]?))\\s*$";
    ////form now on its with 2 operands:
    char *pattLea = "^\\s*(([A-Za-z_][A-Za-z0-9_]+[:])|([A-Za-z_][:]))?\\s*(lea)\\s*(([A-Za-z_][A-Za-z0-9_]+\\[r([1-9]|1[0-5])\\])|([A-Za-z_][A-Za-z0-9_]*))\\s*[,]\\s*(([A-Za-z_][A-Za-z0-9_]+\\[r([1-9]|1[0-5])\\])|([A-Za-z_][A-Za-z0-9_]*)|(r([1-9]|1[0-5])))\\s*$";
    char *pattSub = "^\\s*(([A-Za-z_][A-Za-z0-9_]+[:])|([A-Za-z_][:]))?\\s*(sub)\\s*((([A-Za-z_][A-Za-z0-9_]+\\[r([1-9]|1[0-5])\\])|([A-Za-z_][A-Za-z0-9_]*))|(r([1-9]|1[0-5]))|(#-?[1-8]|#[0-7]?))\\s*,\\s*((([A-Za-z_][A-Za-z0-9_]+\\[r([1-9]|1[0-5])\\])|([A-Za-z_][A-Za-z0-9_]*))|(r([1-9]|1[0-5])))\\s*$";
    char *pattAdd = "^\\s*(([A-Za-z_][A-Za-z0-9_]+[:])|([A-Za-z_][:]))?\\s*(add)\\s*((([A-Za-z_][A-Za-z0-9_]+\\[r([1-9]|1[0-5])\\])|([A-Za-z_][A-Za-z0-9_]*))|(r([1-9]|1[0-5]))|(#-?[1-8]|#[0-7]?))\\s*,\\s*((([A-Za-z_][A-Za-z0-9_]+\\[r([1-9]|1[0-5])\\])|([A-Za-z_][A-Za-z0-9_]*))|(r([1-9]|1[0-5])))\\s*$";
    char *pattMov = "^\\s*(([A-Za-z_][A-Za-z0-9_]+[:])|([A-Za-z_][:]))?\\s*(mov)\\s*((([A-Za-z_][A-Za-z0-9_]+\\[r([1-9]|1[0-5])\\])|([A-Za-z_][A-Za-z0-9_]*))|(r([1-9]|1[0-5]))|(#-?[1-8]|#[0-7]?))\\s*,\\s*((([A-Za-z_][A-Za-z0-9_]+\\[r([1-9]|1[0-5])\\])|([A-Za-z_][A-Za-z0-9_]*))|(r([1-9]|1[0-5])))\\s*$";
    char *pattCmp = "^\\s*(([A-Za-z_][A-Za-z0-9_]+[:])|([A-Za-z_][:]))?\\s*(cmp)\\s*((([A-Za-z_][A-Za-z0-9_]+\\[r([1-9]|1[0-5])\\])|([A-Za-z_][A-Za-z0-9_]*))|(r([1-9]|1[0-5]))|(#-?[1-8]|#[0-7]?))\\s*,\\s*((([A-Za-z_][A-Za-z0-9_]+\\[r([1-9]|1[0-5])\\])|([A-Za-z_][A-Za-z0-9_]*))|(r([1-9]|1[0-5]))|(#-?[1-8]|#[0-7]?))\\s*$";
    ///commant
    char *pattCmt ="^\\s*[;].*$";
    char *pattMac = "^\\s*(macro)\\s*([A-Za-z_][A-Za-z0-9_]*)\\s*$";
    char *pattEndMac = "^\\s*(endm)\\s*$";

    ///Instraction
    char *pattExt = "^\\s*[.](extern)\\s*([A-Za-z_][A-Za-z0-9_]*)\\s*$";
    char *pattEnt = "^\\s*[.](entry)\\s*([A-Za-z_][A-Za-z0-9_]*)\\s*$";
    char *pattData ="\\s*(([A-Za-z_][A-Za-z0-9_]+[:])|([A-Za-z_][:]))?\\s*[.](data)\\s*(-?[0-9]+)(\\s*,\\s*-?[0-9]+)*\\s*$";
    char *pattString = "\\s*(([A-Za-z_][A-Za-z0-9_]+[:])|([A-Za-z_][:]))?\\s*[.](string)\\s*[\"“](.*)[\"”]\\s*$";
    char *pattEmpty = "^[\\s?\\n]$";
    // Array of regex input_string patterns
    char *patterns[] = {pattStop, pattRts, pattClr, pattNot, pattInc, pattInc, pattDec, pattRed, pattJmp, pattBne,
                        pattJsr, pattPrn, pattLea, pattSub, pattAdd, pattMov, pattCmp, pattCmt,pattMac,
                        pattEndMac,pattExt,pattEnt,pattData,pattString, pattEmpty};
    /* does not run propely check asap */
    return 1;
    for (int i = 0; i < NUMPATT; i++) {
        ret = regcomp(&regex, patterns[i], REG_EXTENDED | REG_ICASE);
        if (ret) {
            fprintf(stderr, "Could not compile regex\n");
            return false;
        }
        ret = regexec(&regex, input_string, 0, NULL, 0);
        if(ret==0 && (patterns[i]==pattJmp || patterns[i]==pattBne || patterns[i]==pattJsr)){
            char *pattClr2 = ".*r(1[0-5]|[1-9])\\s*$";
            ret = regcomp(&regex, pattClr2, REG_EXTENDED);
            if (ret) {
                fprintf(stderr, "Could not compile regex\n");
                return 0;
            }
            ret = regexec(&regex, input_string, 0, NULL, 0);
            if (ret == 1){
                ret = 0;
            }
            else
                ret = 1;
        }
        else if(patterns[i]==pattLea && ret == 0){
            char *pattClr2 = ".*r(1[0-5]|[1-9])\\s*[,].*$";
            ret = regcomp(&regex, pattClr2, REG_EXTENDED);
            if (ret) {
                fprintf(stderr, "Could not compile regex\n");
                return 0;
            }
            ret = regexec(&regex, input_string, 0, NULL, 0);
            if (ret == 1){
                ret = 0;
            }
            else
                ret = 1;
        }
        regfree(&regex);
        if (ret == 0) {
            return true; // Return 1 to indicate a match
        }
    }
    return 0; // Return 0 if no match is found
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
    FILE *fp = fopen(file_name, "r+");
    char * buffer = (char *)malloc(sizeof(char) * 82), *  find_sym,  c, *word, *copy, sym_name[74] = {'\0'};
    int counter = 0, curr_line = 1,mem_line = 0, res, status = 1, label_type, ret_val = PASSED, delta = 1;
    int  mode, arr[2],stopped, R_E_type = -1, label_index = 0, turn_on, name_index,  a_indx, l = 0;
    symPTR node, temp;
    mem_mode_node * mmn_node;
    if (!file_name) {
        printf("[!] Error Invalid file name in parse2 stage2\n");
        exit(1);
    }
    *root = NULL;
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
                    mmn_node->opp_mode[0] = arr[0];
                    mmn_node->opp_mode[1] = arr[1];
                    mmn_node->location = curr_line;

                    insert_mmn(mmn_root, mmn_node);

                    delta = calc_delta(arr);
                    if (res != RTS && res != STOP) ++delta; /* for func_t */
                    else if(res == RTS || res == STOP) delta = 1;
                    printf("\nARR[0] = %i\tARR[1] = %i\tDelta: %i\n", arr[0], arr[1], delta);
                }
                /* save helper word's in memory and print current word aka instruction and insert symbol if necessery */
                switch (res) {
                    case LABEL:
                        for(a_indx=0; a_indx<N;a_indx++){
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
                                else{
                                    /* entry */
                                    turn_on = 1;
                                }
                                name_index = stopped;
                                stopped += 7;
                                while(isspace(buffer[stopped])) ++stopped;
                                strcpy(sym_name, &buffer[stopped]);
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
                                    if (R_E_type == EXT && !check_exists(sym_name, root)){
                                        printf("Should create:[%s]\n", sym_name);
                                        /* gives error */
                                        copy = (char *)malloc(strlen(sym_name));
                                        strcpy(copy, sym_name);
                                        node->symbol = copy;
                                        node->baseAddress = 0;
                                        node->offset = 0;
                                        node->value = 0;
                                        node->END_IC = 0;
                                        node->next = NULL;
                                        insert_symTable(root, node);
                                    }
                                    printf("[^] R:%i E:%i\tfor label: [%s]\n", R_E_type == ENT , R_E_type == EXT ,&buffer[label_index]);
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
                                /* // delta = ?; */
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
                        
                        /* get_label_name(); */
                        break;
                    // case CMP:
                    //     /* something 0,1,2,3 */
                    //     printf("CMP \n");
                    //     break;
                    // case PRN:
                    //     /* something 0,1,2,3 */
                    //     printf("PRN\n");
                    //     break;
                    // case MOV:
                    //     printf("MOV\n");
                    //     break;
                    // case ADD:
                    //     printf("ADD\n");
                    //     break;
                    // case SUB:
                    //     printf("SUB\n");
                    //     break;
                    // case LEA:
                    //     printf("LEA\n");
                    //     break;
                    // case CLR:
                    //     printf("CLR\n");
                    //     break;
                    // case NOT:
                    //     printf("NOT\n");
                    //     break;
                    // case DEC:
                    //     printf("DEC\n");
                    //     if (arr[0] == 11){
                    //         printf("0100 REG/NUMBER MODE\n");
                    //     } else if (arr[0] == -1) printf("EXTERN/RELOCATABLE\n");
                    //     else {
                    //         printf("ARR[0] = %i\n", arr[0]);
                    //     }
                    //     break;
                    // case INC:
                    //     printf("INC ?? \n");
                    //     if (arr[0] == 11){
                    //         printf("REG/NUMBER MODE\n");
                    //     } else if (arr[0] == -1) printf("EXTERN/RELOCATABLE\n");
                    //     else {
                    //         printf("ARR[0] = %i\n", arr[0]);
                    //     }
                    //     break;
                    // case RED:
                    //     /* something 1,2,3 */
                    //     printf("RED\n");
                    //     break;
                    // case JMP:
                    //     printf("JMP\n");
                    //     break;
                    // case BNE:
                    //     printf("BNE\n");
                    //     break;
                    // case JSR:
                    //     /* // something  1,2 */
                    //     printf("JSR\n");
                    //     break;
                    // case RTS:
                    //         /* // None */
                    //     printf("RTS\n");
                    //     break;
                    // case STOP:
                    //     /* // None */ 
                    //     printf("STOP\n");
                    //     break;
                    default:
                        printf("OTHER\n");
                        
                }
                for (l = 0; l < delta /* && delta < 5 */; l++){++mem_line;}
            }
        } else{
            wrongString(buffer);
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
int parse2_file_stage_2(char * file_name, int *IC, int *DC, int EXPORT_FILES, symPTR * root, mem_mode_node ** mmn_root){
    FILE *main_fp, *helper_fp;
    FILE *ob_fp;
    
    symPTR curr = *root;
    entPTR * ent_root;
    extPTR * ext_root;
    entPTR ent_node;
    extPTR ext_node;
    char *buffer, *file_name_copy_no_delim = (char *)malloc(strlen(file_name) + 1), *helper_name = (char *)malloc(strlen(file_name) + 1); /* current_char is for .string */
    char *file_name_ob = (char *)malloc(strlen(file_name) + 4), *label_name = (char *)malloc(sizeof(char) * 76);
    char *bin_word, *number_str = (char *)malloc(sizeof(char) * 10), *reg_str = (char *)malloc(sizeof(char) * 10);
    int symbole_type, arr[2] = {0, 0}, opp1 , opp2, line, index, type_dot; /* current_number is for .data */
    int is_instr, recover_index, func_t, mem_addr = 100, write_flag = 1;
    mem_mode_node *mmn_node;
    
    strcpy(file_name_copy_no_delim, file_name);
    
    if (!file_name) {
        printf("[!] Error Invalid file name in parse2 stage2\n");
        exit(1);
    }
    strtok(file_name_copy_no_delim, ".");
    strcpy(file_name_ob, file_name_copy_no_delim);
    strcat(file_name_ob, ".ob");
    ob_fp = fopen(file_name_ob, "w+");
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
    
    
    /* create's the ext and ent files */
    if(file_name_copy_no_delim){
        printToFile_entTable(ent_root, file_name_copy_no_delim);
        printToFile_extTable(ext_root, file_name_copy_no_delim);
    }

    strcpy(helper_name, file_name_copy_no_delim);
    strcat(helper_name, ".help");
    helper_fp = fopen(helper_name, "w+");
    line = 101;
    buffer = (char *)malloc(sizeof(char) * 81);
    while(!feof(main_fp)){
        opp1 = opp2 = 0;
        arr[0] = arr[1] = 0;
        func_t = 0;
        fgets(buffer, 81, main_fp);
        /* in the start of the label/code */
        index = trim(buffer);
        if(buffer[index] == '.'){
            /* // x: prn # */
            type_dot = check_arrtib(buffer, index);
            handle_data(type_dot, helper_fp, buffer, index, root);
        }
        else{
            is_instr = check_inst(buffer);
                if (is_instr-(LABEL+1) >= 0 && is_instr - (LABEL+1) <= 15){
                     bin_word = (char*)malloc(sizeof(char) * 21);
                    strcpy(bin_word, "0100");
                    index += 3; /* after the instr exluding stop */
                    if (is_instr != STOP || is_instr != RTS){
                        strcat(bin_word, intr_bin_str[is_instr - MOV]);
                        fprintf(ob_fp, "[%4i] ", mem_addr++);
                        fputs(word_to_ob_line(bin_word), ob_fp);
                        fputs("\n", ob_fp);        
                    }
                    switch (is_instr) {
                            case CMP:  /* 2 */ 
                                recover_index = index;
                                mmn_node = get_node_by_location(mmn_root, line - 100);
                                if (mmn_node == NULL) {
                                    printf("CRITICAL ERROR\n");
                                    exit(1);
                                }
                                opp1 = mmn_node->opp_mode[0];
                                opp2 = mmn_node->opp_mode[1];
                                /* void handle_two_opps(char * buffer, int func_t, int *mem_addr,int recover_index, char * number_str, char * reg_str, char * label_name, int opp1, int opp2, FILE *ob_fp, symPTR *root) */
                                if (write_flag)
                                    handle_two_opps(buffer, func_t, &mem_addr,recover_index, number_str, reg_str, label_name, opp1, opp2, ob_fp, root);
                                break;
                            case MOV:  /* 2 */
                                recover_index = index;
                                mmn_node = get_node_by_location(mmn_root, line - 100);
                                if (mmn_node == NULL) {
                                    printf("CRITICAL ERROR\n");
                                    exit(1);
                                }
                                opp1 = mmn_node->opp_mode[0];
                                opp2 = mmn_node->opp_mode[1];
                                if (write_flag)
                                    handle_two_opps(buffer, func_t, &mem_addr,recover_index, number_str, reg_str, label_name, opp1, opp2, ob_fp, root);

                                break;
                            /* when copying func_t binary don't forget to - 10 for the correct index in the string array */
                            case ADD:  /* 2 */
                                func_t = 10;
                                recover_index = index;
                                mmn_node = get_node_by_location(mmn_root, line - 100);
                                if (mmn_node == NULL) {
                                    printf("CRITICAL ERROR\n");
                                    exit(1);
                                }
                                opp1 = mmn_node->opp_mode[0];
                                opp2 = mmn_node->opp_mode[1];
                                if (write_flag)
                                    handle_two_opps(buffer, func_t, &mem_addr,recover_index, number_str, reg_str, label_name, opp1, opp2, ob_fp, root);

                                break;
                            case SUB:  /* 2 */
                                func_t = 11;
                                recover_index = index;
                                mmn_node = get_node_by_location(mmn_root, line - 100);
                                if (mmn_node == NULL) {
                                    printf("CRITICAL ERROR\n");
                                    exit(1);
                                }
                                opp1 = mmn_node->opp_mode[0];
                                opp2 = mmn_node->opp_mode[1];
                                if (write_flag)
                                    handle_two_opps(buffer, func_t, &mem_addr,recover_index, number_str, reg_str, label_name, opp1, opp2, ob_fp, root);

                                break;
                            case LEA:  /* 2 */
                                recover_index = index;
                                mmn_node = get_node_by_location(mmn_root, line - 100);
                                if (mmn_node == NULL) {
                                    printf("CRITICAL ERROR\n");
                                    exit(1);
                                }
                                opp1 = mmn_node->opp_mode[0];
                                opp2 = mmn_node->opp_mode[1];
                                if (write_flag)
                                    handle_two_opps(buffer, func_t, &mem_addr,recover_index, number_str, reg_str, label_name, opp1, opp2, ob_fp, root);
                                break;
                            case PRN:  /* 1 */
                                if (write_flag)
                                    handle_one_opp(buffer, func_t , &mem_addr, recover_index, number_str, reg_str, label_name, opp1, ob_fp, root);
                                break;
                            case CLR:  /* 1 */
                                func_t = 10;
                                if (write_flag)
                                    handle_one_opp(buffer, func_t , &mem_addr, recover_index, number_str, reg_str, label_name, opp1, ob_fp, root);
                                break;
                            case NOT:  /* 1 */
                                func_t = 11;
                                if (write_flag)
                                    handle_one_opp(buffer, func_t , &mem_addr, recover_index, number_str, reg_str, label_name, opp1, ob_fp, root);
                                break;
                            case DEC:  /* 1 */
                                func_t = 13;
                                if (write_flag)
                                    handle_one_opp(buffer, func_t , &mem_addr, recover_index, number_str, reg_str, label_name, opp1, ob_fp, root);
                                break;
                            case INC:  /* 1 */
                            if (write_flag)
                                    handle_one_opp(buffer, func_t , &mem_addr, recover_index, number_str, reg_str, label_name, opp1, ob_fp, root);
                                func_t = 12;
                                break;
                            case RED:  /* 1 */
                            if (write_flag)
                                    handle_one_opp(buffer, func_t , &mem_addr, recover_index, number_str, reg_str, label_name, opp1, ob_fp, root);
                                break;
                            case JMP:  /* 1 */
                                func_t = 10;
                            if (write_flag)
                                    handle_one_opp(buffer, func_t , &mem_addr, recover_index, number_str, reg_str, label_name, opp1, ob_fp, root);
                                break;
                            case BNE:  /* 1 */
                                func_t = 11;
                            if (write_flag)
                                    handle_one_opp(buffer, func_t , &mem_addr, recover_index, number_str, reg_str, label_name, opp1, ob_fp, root);
                                break;
                            case JSR:  /* 1 */
                                func_t = 12;
                            if (write_flag)
                                    handle_one_opp(buffer, func_t , &mem_addr, recover_index, number_str, reg_str, label_name, opp1, ob_fp, root);
                                break;
                            case RTS:  /* 0 */
                                strcpy(bin_word, intr_bin_str[14]);
                                if (write_flag) {
                                    fprintf(ob_fp, "[%4i] ", mem_addr++);
                                    fputs(word_to_ob_line(bin_word), ob_fp);
                                    fputs("\n", ob_fp);
                                }
                                break;
                            case STOP:  /* 0 */
                                strcpy(bin_word, intr_bin_str[15]);
                                if (write_flag) {
                                    fprintf(ob_fp, "[%4i] ", mem_addr++);
                                    fputs(word_to_ob_line(bin_word), ob_fp);
                                    fputs("\n", ob_fp);
                                }
                                write_flag = 0;
                                break;
                            default:
                                printf("UNKNOWN\n");
                                break;
                    } 
                } else { /* it's a label */
                    index = 0;
                    while(buffer[index] != '.') ++index;
                    type_dot = check_arrtib(buffer, index);
                    handle_data(type_dot, helper_fp, buffer, index, root);
                    printf("LABEL: %s\n", buffer);
                }
        }
        
        /* printf("[LINE: %i] %s\n--------------------\n", line, buffer); */         
        ++line;
        buffer[0] = '\0';
        /* check if the line is valid or not */
    }

    fclose(helper_fp);
    free(buffer);
    
    helper_fp = fopen(helper_name, "r");
    if (!helper_fp){
        printf("[/] Error opening file in parse2 stage2\n");
        exit(1);
    }
    
    buffer = (char *)malloc(80);
    
    while(!feof(helper_fp)){
        ++(*DC);
        fgets(buffer, 80, helper_fp);
        fprintf(ob_fp, "[%4i] %s", mem_addr++, buffer);
    }

    fprintf(ob_fp, "IC: %i\tDC: %i\n", *IC, *DC);

    fclose(ob_fp);
    fclose(main_fp);
    fclose(helper_fp);
    free(label_name);
    free(helper_name);
    free(file_name_copy_no_delim);
    free(file_name_ob);
    free(buffer);
    return 1;
}

/**
 * @brief this function check's for the type of the label by checking the buffer
 * 
 * @param buffer the buffer to look inside of
 * @param label_index the start index of the label  
 * @return int 
 */
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
 * @brief check's if the given label name exist's inside the symTable
 * 
 * @param name_to_search name to search
 * @param root  the symTable
 * @return int 
 */
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

/**
 * @brief handles the writing of .data and .string to the ob file using an helper file
 * 
 * @param type_dot 
 * @param helper_fp 
 * @param buffer 
 * @param index 
 * @param root 
 */
void handle_data(int type_dot, FILE * helper_fp, char * buffer, int index, symPTR *root){
    int index_bkup, name_len, current_number, minus_flag = 0;
    char *sym_name, current_char, *num_bin_str, *line, *hex_word;
    line = (char *)malloc(sizeof(char) * 21);
    switch (type_dot) {
                case EXT:
                    /* printf("type: EXT, %s\n", &buffer[index]);
                     * put zero inside the symTable if you find the label
                     */
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
                    /* 2 .string "ancndvncdv" -> helper file */
                    break;
                case DATA:
                    index += 5;
                    /* printf("type: DATA, buffer = %s\n", &buffer[index]);
                    * 1 .data 1,2            ,2         ,2, -2,2,2,2,2,2 ->  helper file
                    */
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
    int i = 0, ret_val = OTHER, index = 0, comma_count = 0, flag;
    char * reg_name = NULL, *buffer_to_check = (char*)malloc(strlen(buffer) + 2),str[4], reg[4], skip = 0;
    if (sizeof(info_arr) != sizeof(int) * 2) return -1;
    strcpy(buffer_to_check, buffer);
    strcat(buffer_to_check, " ");
    if (strstr(buffer_to_check, "stop") == NULL){
        while(isspace(buffer_to_check[index]) && buffer_to_check[index] != ',' )++index;
        if (buffer_to_check[index] == ',') return COMMENT;
        index += 3;
        while(isspace(buffer_to_check[index])) ++index;
        if (buffer_to_check[index] == ',') return COMMENT;
        
        if (buffer_to_check[index] == '#') info_arr[0] = num_addr;
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
            info_arr[1] = num_addr;
        }
        for(i = 0; i < 3; ++i){
            if (buffer_to_check[index + i] != ',' && buffer_to_check[index + i] != ' ') reg[i] = buffer_to_check[index + i];
            else break;
        }
        reg[i] = '\0';
        index += i;
        if (buffer[index] == ',' ) ret_val = COMMA;
        if ((buffer_to_check[index] == ' ' || buffer_to_check[index] == '\n' || buffer_to_check[index] == '\0') && !skip) {
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
    char * name = (char *)malloc(80);
    *stopped = 0;

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
    int bits_index = 15;
    strcpy(bits, "0000000000000000");
    while ( un_number > 0 ) {
        bits[bits_index--] = (un_number & 1) + '0';
        un_number = ( un_number >> 1);
    }
    bits[16] = '\0';
    return bits;
}

void handle_label(FILE *ob_file, symPTR *root, char *label, int * mem_addr){
    symPTR temp = *root, work = NULL;
    char *help_word = (char *)malloc(sizeof(char)  * 21);
    
    while(temp){
        if (strcmp(temp->symbol, label) == 0){
            work = temp;
            break;
        }
        temp = temp->next;
    }
    if (work == NULL) {
        printf("No such symbol!!\n");
        exit(1);
    }
    
    if (work->arr[0] == 1){
        /* external */
        fprintf(ob_file,"[%4i] A1-B0-C0-D0-E0\n", (*mem_addr)++);
        fprintf(ob_file,"[%4i] A1-B0-C0-D0-E0\n", (*mem_addr)++);
        return;
    }

    else {
        /* FIRST WORD IS BASE THE OTHER IS OFFSET */
        strcpy(help_word, "0010");
        strcat(help_word, num_to_bin_str(work->baseAddress));
        /* printf("HELP WORD 1 = [%s]\n", word_to_ob_line(help_word)); */
        fprintf(ob_file,"[%4i] ", (*mem_addr)++);
        fputs(word_to_ob_line(help_word), ob_file);
        fputs("\n", ob_file);

                
        strcpy(help_word, "0010");
        strcat(help_word, num_to_bin_str(work->offset));
        /* printf("HELP WORD 2 = [%s]\n", word_to_ob_line(help_word)); */
        fprintf(ob_file,"[%4i] ", (*mem_addr)++);
        fputs(word_to_ob_line(help_word), ob_file);
        fputs("\n", ob_file);

        return;
    }
}

void handle_one_opp(char * buffer, int func_t , int *mem_addr,int recover_index, char * number_str, char * reg_str, char * label_name, int opp1, FILE *ob_fp, symPTR *root){
    int local_index = 0;
    char *bin_word = (char *)malloc(sizeof(char) * 21);

    if (opp1 == direct_addr){
        strcpy(bin_word, "0100");
        if (func_t)
            strcat(bin_word, funct_str[func_t - 10]);
        else
            strcat(bin_word, "0000");

        get_reg_str(1, recover_index, buffer, reg_str);
        while(buffer[recover_index] != ',') ++recover_index;
        ++recover_index;
        strcat(bin_word, reg_str);
        strcat(bin_word, "11");

        strcat(bin_word, "0000");
        strcat(bin_word, "11");
        printf("[%s]\n", (bin_word));
        printf("[%s]\n", word_to_ob_line(bin_word));
        
        fprintf(ob_fp, "[%4i] ", (*mem_addr)++);
        fputs(word_to_ob_line(bin_word), ob_fp);
        fputs("\n", ob_fp);
        return;
    }
    
    switch (opp1) {
            case num_addr:
                get_num_str(1, recover_index, buffer, number_str);
                while(buffer[recover_index] != ',') ++recover_index;
                ++recover_index;
                printf("number:%s, negative: %i\n", number_str, number_str[0] == '1');
                handle_number(ob_fp, number_str, mem_addr);
                break;
            case direct_addr:
                /* check  */
                get_reg_str(1, recover_index, buffer, reg_str);
                while(buffer[recover_index] != ',') ++recover_index;
                ++recover_index;
                printf("reg, bin_str: %s\n", reg_str);
                handle_register(ob_fp, reg_str, mem_addr);
                break;
            case OTHER:
                while(buffer[recover_index] != ',' && buffer[recover_index] != ' '){
                    label_name[local_index] = buffer[recover_index];
                    ++recover_index;
                    ++local_index;
                }
                ++recover_index;
                label_name[local_index] = '\0';
                printf("need to check = [%s]\n", label_name);
                handle_label(ob_fp, root, label_name, mem_addr);
                break;
            default:
                printf("WTF1?\n");
                break;
    }
}


/**
 * @brief this function handles the case of two operands as the name suggests
 *        it checks for the operand type using the mem_mode_node list  
 *        and depend's on the output to write the coresponding word to the ob file
 * 
 * @param buffer        the current line to handle
 * @param func_t        the func_t type of the current line
 * @param recover_index the index given by the sending function to indicate the start of the operands
 * @param number_str    the buffer for the case number
 * @param reg_str       the buffer for the case register
 * @param label_name    the buffer for the case label
 * @param opp1          type of opp1
 * @param opp2          type of opp2
 * @param ob_fp         the file pointer of the object file
 * @param root          the manager of the symTable
 */
void handle_two_opps(char * buffer, int func_t, int *mem_addr,int recover_index, char * number_str, char * reg_str, char * label_name, int opp1, int opp2, FILE *ob_fp, symPTR *root){
    int local_index = 0;
    char *bin_word = (char *)malloc(sizeof(char) * 21);
    while(isspace(buffer[recover_index])) ++recover_index;

    if (opp1 == direct_addr && opp2 == direct_addr){
        strcpy(bin_word, "0100");
        if (func_t)
            strcat(bin_word, funct_str[func_t - 10]);
        else
            strcat(bin_word, "0000");

        get_reg_str(1, recover_index, buffer, reg_str);
        while(buffer[recover_index] != ',') ++recover_index;
        ++recover_index;
        printf("reg, bin_str: %s\n", reg_str);
        strcat(bin_word, reg_str);
        strcat(bin_word, "11");
        while(isspace(buffer[recover_index])) ++recover_index;                                
        get_reg_str(2, recover_index, buffer, reg_str);
        strcat(bin_word, reg_str);
        strcat(bin_word, "11");
        printf("[%s]\n", (bin_word));
        printf("[%s]\n", word_to_ob_line(bin_word));
        
        fprintf(ob_fp, "[%4i] ", (*mem_addr)++);
        fputs(word_to_ob_line(bin_word), ob_fp);
        fputs("\n", ob_fp);
        return;
    }
        switch (opp1) {
            case num_addr:
                get_num_str(1, recover_index, buffer, number_str);
                while(buffer[recover_index] != ',') ++recover_index;
                ++recover_index;
                printf("number:%s, negative: %i\n", number_str, number_str[0] == '1');
                handle_number(ob_fp, number_str, mem_addr);
                break;
            case direct_addr:
                /* check  */
                get_reg_str(1, recover_index, buffer, reg_str);
                while(buffer[recover_index] != ',') ++recover_index;
                ++recover_index;
                printf("reg, bin_str: %s\n", reg_str);
                handle_register(ob_fp, reg_str, mem_addr);
                break;
            case OTHER:
                while(buffer[recover_index] != ',' && buffer[recover_index] != ' '){
                    label_name[local_index] = buffer[recover_index];
                    ++recover_index;
                    ++local_index;
                }
                ++recover_index;
                label_name[local_index] = '\0';
                printf("need to check = [%s]\n", label_name);
                handle_label(ob_fp, root, label_name, mem_addr);
                break;
            default:
                printf("WTF1?\n");
                break;
        }
        while(isspace(buffer[recover_index])) ++recover_index;                                
        switch (opp2) {
            case num_addr:
                get_num_str(2, recover_index, buffer, number_str);
                printf("number:%s, negative: %i\n", number_str, number_str[0] == '1');
                handle_number(ob_fp, number_str, mem_addr);
                break;
            case direct_addr:
                /* check  */
                get_reg_str(2, recover_index, buffer, reg_str);
                printf("reg, bin_str: %s\n", reg_str);
                handle_register(ob_fp, reg_str, mem_addr);
                break;
            case OTHER:
                local_index = 0;
                while(buffer[recover_index] != '\n' && buffer[recover_index] != ' ' && buffer[recover_index] != '\0'){
                    label_name[local_index] = buffer[recover_index];
                    ++recover_index;
                    ++local_index;
                }
                label_name[local_index] = '\0';
                printf("need to check = [%s]\n", label_name);
                handle_label(ob_fp, root, label_name, mem_addr);
                break;
            default:
                printf("WTF2?\n");
                break;
        }
}