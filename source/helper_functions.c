#include "../include/helper_functions.h"
#include "../include/first_parse.h"

int trim(char * str_to_cut){
    int len = strlen(str_to_cut), i = 0;
    while(i < len && isspace(str_to_cut[i])) ++i;
    return i;
}

int alpha_beta(char c){
    return (c <= 'Z' && c >= 'A') || (c <= 'z' && c >= 'a');
}

bool check_regs(char *buffer, int *index){
    bool ret_val = false;
    if ((buffer[0] <= '9' && buffer[0] >= '0') && (buffer[1] == ' ' || buffer[1] == ',' || buffer[1] == '\0' || buffer[1] == '\n')){
        /* reg is r0 -> r9 */
        ++(*index);
        ret_val = true;
    }
    if ((buffer[0] == '1') && (buffer[1] >= '0' || buffer[1] <= '5') && (buffer[2] == ' ' || buffer[2] == ',' || buffer[2] == '\0' || buffer[2] == '\n')){
        /* reg is r10 -> r15 */
        ++(*index);
        ++(*index);
        ret_val = true;
    }
    return ret_val;
}

bool handle_cmp(char * instr){
    bool ret_val = true, type; 
    int index = trim(instr), len = strlen(instr), counter_hashes=0;
    // cmp
    while(index < len && index < 80){
        if(instr[index] == '#'){
            /* send to helper function */
        }
        else if ( instr[index] != 'r' && alpha_beta(instr[index]) ){
            /* send to helper function that checks the label in the symTable checks until ' ' || ',' */
            // HELLO  .data 1, 2, 3
            /* (&instr[index]); */       
        }
        else if (instr[index] == 'r'){
            /* check first_and_second */
            // sub r1, r2
            type = check_regs(&instr[index + 1], &index);
            if (type)
                printf("1 This is reg\n");
            else
                printf("1 This is label or variable\n");
            
            index += trim(&instr[index]);
            if (instr[index] == ',') ++index;
            index += trim(&instr[index]);
            if (instr[index] == 'r'){
                type = check_regs(&instr[index + 1], &index);
                if (type)
                    printf("2 This is reg\n");
                else
                    printf("2 This is label or variable\n");
            }else {
                printf("2 This is label or variable\n");
            }
        }
        ++index;
    }
    
    return ret_val;
}

bool handle_two_opps(char *buffer){
    bool ret_val = true; 
    int index = 0, len = strlen(buffer), counter_hashes=0;
    if (!isspace(buffer[index])) ret_val = false;
    while(index < len && ret_val){
        if(!isspace(buffer[index])){
            if(buffer[index] == '#'){
                if( counter_hashes<=2)
                    ++counter_hashes;
                else
                    ret_val = false;
            }
            else {
                
            }
        }
        ++index;
    }
    return ret_val;
}

bool handle_zero(char * instr){
    int len = strlen(instr), i = 0;
    bool ret_val = true;    
    while(i < len){
        if (!isspace(instr[i])) ret_val = false;
        ++i;
    }
    return ret_val;
}



char * convert_num_to_bin_str(char * find_num){
    // A - B - {C D E} 
    // plus instr name 
    int counter_hashes = 0, trim_index = trim(find_num) + 3;
    int instr_type = check_inst(find_num);
    switch (instr_type) {
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
        case STOP:
            ++trim_index;
            // None
            break; 
            
    }
    return NULL;
}

char * convert_instr_to_bin_str(char * instr);
// A - {B} - C - D - E
// A - B B B B
// B = {0 | 0 | 0 | 0 | 0 | 1 | 0 | 0 | 0 | 0 | 0 }
// HELP WORD
// לעשות enum עם מילות עזר ומצבים שונים לשריון מיליפ בזיכרון

char * convert_operand_to_bin_str(char * opp);
// NIBLE A - B - {C D E}

char * get_addresing_mode(char * instr);
// A = [ 0 { A R E } ]