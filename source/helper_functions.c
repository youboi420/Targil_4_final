#include "../include/helper_functions.h"
#include "../include/first_parse.h"

#define LINE 21

char hex_map[] ={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

int trim(char * str_to_cut){
    int len = strlen(str_to_cut), i = 0;
    while(i < len && isspace(str_to_cut[i])) ++i;
    return i;
}

char * word_to_ob_line(char * word){
    char * ob_line = (char *)malloc(sizeof(char) * LINE);
    int i = 0, j = i, sum = 0x0, buffer_index = 0;
    unsigned int mask;
    
    if(!word){
        return NULL;
    }

    for (i = 0; i <= 16; i += 4){
        sum = mask = 0;
        for(j = 3; j >= 0; --j){
            if (! word[i + j]) return NULL;
            mask = (word[i + j]=='1') ? 1 : 0;
            if (mask){
                mask <<= (3 - j);
            }
            sum |= mask;
        }
        if (i > 0){
            ob_line[buffer_index++] = (char)('A' + (int)(i/4));
        }
        else{
            ob_line[buffer_index++] = (char)('A');
        }
        ob_line[buffer_index++] = hex_map[sum];
        ob_line[buffer_index++] = '-';
    }
    ob_line[14] = '\0';
    return ob_line;
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
    if (buffer[1]){    
        if ((buffer[0] == '1') && (buffer[1] >= '0' || buffer[1] <= '5') && (buffer[2] == ' ' || buffer[2] == ',' || buffer[2] == '\0' || buffer[2] == '\n')){
            /* reg is r10 -> r15 */
            ++(*index);
            ++(*index);
            ret_val = true;
        }
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
    int trim_index = trim(find_num) + 3;
    int instr_type = check_inst(find_num);
    switch (instr_type) {
        case CMP:
            
        case PRN:
            /* // something 0,1,2,3 */
            
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
            /* something 1,2,3 */
            break;
        case JMP:
        case BNE:
        case JSR:
            /* something 1,2 */
            break;
        /* None */
        case RTS:
            break;
        case STOP:
            ++trim_index;
            /* None */
            break; 
            
    }
    return NULL;
}

char * convert_instr_to_bin_str(char * instr);
/* // A - {B} - C - D - E
// A - B B B B
// B = {0 | 0 | 0 | 0 | 0 | 1 | 0 | 0 | 0 | 0 | 0 }
// HELP WORD
// לעשות enum עם מילות עזר ומצבים שונים לשריון מיליפ בזיכרון */

char * convert_operand_to_bin_str(char * opp);
/* // NIBLE A - B - {C D E} */

char * get_addresing_mode(char * instr);
/* // A = [ 0 { A R E } ] */