#include "../include/first_parse.h"
#include "../include/my_error.h"
#include "../include/utils.h"


/**
 * @brief check if there is any assembly instructions in the given buffer
 * 
 * @param buffer the buffer to search inside it
 * @return int from the ENUM of modes/ret_val
 */
int check_inst(char * buffer){
    int ret_val = 0, index = 0, i = 0;
    char INST[4];
    while(isspace(buffer[index])) ++index;
    
    if (buffer[index] == ';') return COMMENT;

    
    for(i = 0; i < 3; ++i){
        INST[i] = buffer[index + i];
    }
    INST[3] = '\0';
    
    if (buffer[index + i] != 'p' || buffer[index + i] != ' ') ret_val = OTHER;

    if (strcmp(INST, ":") == 0){
        ret_val = LABEL;
    }
    else if (strcmp(INST, "mov") == 0){ /* 1,2,3 */
        ret_val = MOV;
    }
    else if (strcmp(INST, "cmp") == 0){ /* 0,1,2,3 */ 
        ret_val = CMP;
    }
    else if (strcmp(INST, "add") == 0){ /* 1,2,3 */
        ret_val = ADD;
    }
    else if (strcmp(INST, "sub") == 0){ /* 1,2,3 */
        ret_val = SUB;
    }
    else if (strcmp(INST, "lea") == 0){ /* 1,2,3 */
        ret_val = LEA;
    }
    else if (strcmp(INST, "clr") == 0){ /* 1,2,3 */
        ret_val = CLR;
    }
    else if (strcmp(INST, "not") == 0){ /* 1,2,3 */
        ret_val = NOT;
    }
    else if (strcmp(INST, "inc") == 0){ /* 1,2,3 */
        ret_val = INC;
    }
    else if (strcmp(INST, "dec") == 0){ /* 1,2,3 */
        ret_val = DEC;
    }
    else if (strcmp(INST, "jmp") == 0){ /* 1,2 */
        ret_val = JMP;
    }
    else if (strcmp(INST, "bne") == 0){ /* 1,2, */
        ret_val = BNE;
    }
    else if (strcmp(INST, "jsr") == 0){ /* 1,2, */
        ret_val = JSR;
    }
    else if (strcmp(INST, "red") == 0){ /* 1,2,3 */
        ret_val = RED;
    }
    else if (strcmp(INST, "prn") == 0){ /* 0,1,2,3 */
        ret_val = PRN;
    }
    else if (strcmp(INST, "rts") == 0){ /* NONE */
        ret_val = RTS;
    }

    else if (strcmp(INST, "sto") == 0 && buffer[index + i] == 'p'){
        ret_val = STOP;
    }
    else ret_val = OTHER;
    
    return ret_val;
}

/**
 * @brief Get's the macro name from a macro defiend line if mode is 1 else just finds the name 
 * 
 * @param buffer the target to find the macro's name inside it  
 * @return char* the macro's name buffer
 */
char* get_macro_name(char * buffer, int mode){
    /* the buffer is on the first address */
    char *temp, *macro_name;
    int indx = 0, local_indx = 0, size = 1;
    while(isspace(buffer[indx])) ++indx;

    /* it means were on the word [m]acro on the letter (m) */
    if (mode == 1) indx += 5;

    while(isspace(buffer[indx])) ++indx;
    macro_name = (char *)malloc(size);
    /* while reading macro name */
    while(! isspace(buffer[indx]) && buffer[indx] != '\n'){
        macro_name[local_indx] = buffer[indx];
        ++local_indx;
        ++indx;
        if (local_indx == size){
            size *= 2;
            temp = (char*)realloc(macro_name, sizeof(char) * size);
            if (temp == NULL){
                return NULL;
            }
            macro_name = temp;
        }
    }
    macro_name[local_indx] = '\0';
    return macro_name;
}

/**
 * @brief creates the .am file
 *      opens the given file name and replaces all the macros defentions with empty lines and the calls
 *      of the macro's to the content itself  
 * @param file_name used to open the file and write the new .proccessed file
 */
int macro_proccess(char *file_name){
    FILE *fp;
    FILE *w_file;
    char parsed_filename[strlen(file_name) + 4], *buffer, *macro_name = NULL, *macro_buff, *macro_content = (char* )malloc(LIMIT), *saved_content;
    int i = 0, size = 1, stop = 0, length = 0, line = 0;
    ENTRY ent, *p_ent;
     /* ------------ get macro parsed name ------------ */
    char macro_file_name[strlen(file_name) + 4]; 
    strcpy(macro_file_name, file_name);
    strcat(macro_file_name, ".as");
    fp = fopen(macro_file_name, "r+");
    
    /* ------------ get macro parsed name ------------ */
    strcpy(parsed_filename, file_name);
    strcat(parsed_filename, ".am");
    w_file = fopen(parsed_filename, "w+");

    
    if (!fp || !w_file){
        printf("[!] File not opened...\n FP: %p\tW_FILE: %p", fp, w_file);
        return FILE_EXCEPTION;
    }
    printf("working on file: %s | file_ptr: %p\n", file_name, fp);
    while(!feof(fp)){
        buffer = (char*)malloc(sizeof(char) * LIMIT);
        while (fgets(buffer, LIMIT, fp)) {
            stop = 0;
            size = 1;
            if (strstr(buffer, "macro") != NULL){
                macro_name = get_macro_name(buffer, 1);
                if (macro_name == NULL){
                    printf("[LINE: %i] Invalid macro name\n", line);
                    return MACRO_EXCEPRION;
                }
                /* we iterate untill endm */

                while( !stop ){
                    macro_buff = (char*)malloc(sizeof(char) * LIMIT);
                    if (!fgets(macro_buff, LIMIT, fp)){
                        free(macro_buff);
                        break;
                    }
                    if (strstr(macro_buff, "endm") != NULL) {
                        stop = 1;
                        break;
                    }
                    if (size == 1){
                        strcpy(macro_content, macro_buff);
                    }
                    else 
                        strcat(macro_content, macro_buff);
                    size *= 2;
                    length += strlen(macro_buff);

                    if (size * LIMIT <= length) macro_content = (char *)realloc(macro_content,  size);
                }
                ent.key = (void*)macro_name;
                /* save content */
                saved_content = (char*)malloc(strlen(macro_content) + 1);
                strcpy(saved_content, macro_content);
                ent.data = saved_content; 
                p_ent = hsearch(ent, FIND);
                if (p_ent == NULL){
                    p_ent = hsearch(ent, ENTER);
                    if (p_ent == NULL){
                        printf("[$] entery failed...\n");
                    }
                }
                
                /* if (! instruction( ) -> insert the macro of current line ("            m1              ") ) */
                printf("[#] Macro's %s content:\n%s", macro_name, saved_content);
                macro_content[0] = '\0'; /* for next iteration */
            }
            else if(check_inst(buffer) == -1){
                macro_name = get_macro_name(buffer, 2);
                ent.key = (void*)macro_name;
                p_ent = hsearch(ent, FIND);
                if (p_ent != NULL){
                    // printf("[!] Found macro %s, content: \n%s\n", macro_name, (char*)p_ent->data);
                    fputs((char*)p_ent->data, w_file);
                }
                else{
                    fputs(buffer, w_file);
                    printf("Macro: %s not found\n", macro_name);
                }
                // fputs(buffer, w_file);
                // printf("buffer? : %s", buffer);
            }
            else{
                fputs(buffer, w_file);
            }
            ++line;
        }   
        free(macro_content);
        free(macro_buff);
        
    }
    free(saved_content);
    free(buffer);
    fclose(fp);
    fclose(w_file);
    return PASSED;
}