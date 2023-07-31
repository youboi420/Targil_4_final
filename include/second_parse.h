#include "../include/my_error.h"
#include "entTable.h"
#include "extTable.h"
#include "symTable.h"

#define NUMPATT 25
typedef enum OPP_TYPE {type_reg = 200, type_num, type_ext, type_ent} OPP_TYPE;
typedef enum ARE_MODES{NONE = -100, A_flag = 1001, R_flag, E_flag} ARE_MODES;
typedef struct mem_mode_node{
    int location;
    int opp_mode[2];
    struct mem_mode_node *next;
} mem_mode_node;

void read_to_symTable(symPTR node);
void print_mmn_list(mem_mode_node**root);
void create_ob_file(symPTR *manager, FILE *main_fp,char * file_name);
void ext_put_zero(char * node_name, symPTR *root);

/**
 * @brief handles the writing of .data and .string to the ob file using an helper file
 *        which is later added to the file_name.ob file in the function stage 2
 *
 *        if the type is .string we go threw the letters between the two paranthesis "this is en example"
 *        we iterate for each letter and write it's ascii code to a word in memory...
 *
 *        if the type is .data we go threw the buffer between the first number and the last number
 *        for example 1,2,3 each number will get it's word in memory 
 *        here is the template for the the ascii codes / numbers in memory
 *              a - [0100|0000|0000|0110|0001]
 *              b - [0100|0000|0000|0110|0010]
 *              c - [0100|0000|0000|0110|0011]
 *
 *              1 - [0100|0000|0000|0000|0001]
 *              2 - [0100|0000|0000|0000|0010]
 *              3 - [0100|0000|0000|0000|0011]
 *       
 * @param type_dot the type of the given buffer possible are .data and .string
 * @param helper_fp the file pointer to write to, which is sent from stage 2. it does it this way so we dont need a new file pointer everytime
 * @param buffer the current line which is being proccesed
 * @param index the index of the start of that DATA
 * @param root the linked lits of the symbols for putting zero's after were done with the extern labels
 */
void handle_data(int type_dot, FILE *helper_fp, char * buffer,int index, symPTR *root);
void handle_one_opp(char * buffer, int func_t , int *mem_addr,int recover_index, char * number_str, char * reg_str, char * label_name, int opp1, FILE *ob_fp, symPTR *root);
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
void handle_two_opps(char * buffer, int func_t, int *mem_addr,int recover_index, char * number_str, char * reg_str, char * label_name, int opp1, int opp2, FILE *ob_fp, symPTR *root);
void handle_label(FILE *ob_file, symPTR *root, char *label, int *mem_addr);

/**
 * @brief this function check's for the type of the label by checking the buffer
 * 
 * @param buffer the buffer to look inside of
 * @param label_index the start index of the label  
 * @return int 
 */
int check_R_E(char * buffer, int *label_index);
int valid_line(char * buffer);
int calc_delta(int arr[2]);

/**
 * @brief check's if the given label name exist's inside the symTable
 * 
 * @param name_to_search name to search
 * @param root  the symTable
 * @return int 
 */
int check_exists(char * name_to_search, symPTR * root);
int check_arrtib(char *buffer, int index);
int check_arrtib(char *buffer, int start_index);
int get_opp_type(char * buffer, int *indx);
int check_exists(char * name_to_search, symPTR * root);
int mem_mode_info(char *buffer, int *info_arr);
int calc_base_addr(int addr);
int update_symbole(symPTR *manager, int ic, void * new_data, int data_type);

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
int parse2_file_stage_1(char * file_name, int *IC, int *DC, symPTR * root, mem_mode_node** mmn_root);

/**
 * @brief this function parses the file for the last time before outputing the requested files
 *        it uses the mmn list to get the types of the operands if neccery it opens the given file name 
 *        it uses the symbole table to get all the extern and all the entry label's then from that it create's the extern file which is file_name.ext 
 *        and the the entrys file file_name.ent
 *        after it takes care of the symbole table it parses the file for the last time. making use of the symbole table as instructioned
 *        it follows the rules of outputing the correctly writen object file which is file_name.ob
 *        and for that it uses the EXPORT_FILES flag as guidens        
 *        
 * @param file_name the file to parse
 * @param IC the inctruction counter
 * @param DC the data counter
 * @param EXPORT_FILES the flag wheter to export or not
 * @return int 
 */
int parse2_file_stage_2(char * file_name, int *IC, int *DC,  int EXPORT_FILES, symPTR * root, mem_mode_node** mmn_root);


char * num_to_bin_str(int number);

/**
 * @brief this function checks for symbol's/variable name inside the given buffer and inc's the given stopped cariable pointer 
 * 
 * @param buffer the string to look for a label's name inside
 * @param stopped  the pointer to the stopped index
 * @return char* 
 */
char * find_symbol(char * buffer, int *stopped);