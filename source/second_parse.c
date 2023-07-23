#include "../include/second_parse.h"
#include "../include/utils.h"
#include <stdio.h>
#include <string.h>
#include <regex.h>

int parse2_file_stage_1(char * file_name){
    
    if (!file_name) {
        printf("[!] Error Invalid file name in parse2 stage2\n");
        exit(1);
    }
    FILE *fp = fopen(file_name, "r+");
    char * buffer = (char *)malloc(sizeof(char) * 82), c;
    int counter = 0, line = 0, res;
    
    if (!fp){
        printf("[/] Error opening file in parse2 stage2\n");
        exit(1);
    }

    /*  בדיקה של רגקס על כל שורה 
     *  סינון הקלט לפי סוגו למשל מספר אופרדנים סוגי מיעון וכו
     *  לאחר מכן צריכים לסנן לטבלת הסמלים ולכתוב מילים שמורות בזיכרון התוכנית (כלומר לשריין) אחרי זאת לסגור את הקובץ 
    */
    while(!feof(fp)){
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
        printf("[LINE: %i]\t%s\n\tLine info  ", 100 + line,buffer);
        res = get_mode(buffer);
        ++line;
    }
    return PASSED;
}

int parse2_file_stage_2(char * file_name){
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