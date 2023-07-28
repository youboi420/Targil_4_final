#include <stdbool.h>
#include <ctype.h>
#include <string.h>

typedef enum addresing_mode  {immediate_addr = 10, direct_addr, indx_addr, reg_addr} addresing_mode;

int trim(char * str_to_cut);
bool handle_cmp(char * instr);
bool check_regs(char * buffer, int *index);
char * convert_instr_to_bin_str(char * instr);
char * convert_operand_to_bin_str(char * opp);
char * get_addresing_mode(char * instr);
char * word_to_ob_line(char * word);