#include <stdbool.h>
#include <ctype.h>
#include <string.h>

typedef enum addresing_mode  {immediate = 10, direct, indx, reg} addresing_mode;

bool handle_cmp(char * instr);
bool  check_regs(char * buffer, int *index);
char * convert_num_to_bin_str(char * num);
char * convert_instr_to_bin_str(char * instr);
char * convert_operand_to_bin_str(char * opp);
char * get_addresing_mode(char * instr);
char * word_to_ob_line(char * word);