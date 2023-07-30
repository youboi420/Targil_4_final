#include "../include/my_error.h"
#include "entTable.h"
#include "extTable.h"
#include "symTable.h"

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
void handle_data(int type_dot, FILE *helper_fp, char * buffer,int index, symPTR *root);
void handle_one_opp(char * buffer, int func_t , int *mem_addr,int recover_index, char * number_str, char * reg_str, char * label_name, int opp1, FILE *ob_fp, symPTR *root);
void handle_two_opps(char * buffer, int func_t, int *mem_addr,int recover_index, char * number_str, char * reg_str, char * label_name, int opp1, int opp2, FILE *ob_fp, symPTR *root);
void handle_label(FILE *ob_file, symPTR *root, char *label, int *mem_addr);

int check_R_E(char * buffer, int *label_index);
int valid_line(char * buffer);
int calc_delta(int arr[2]);
int check_exists(char * name_to_search, symPTR * root);
int check_arrtib(char *buffer, int index);
int check_arrtib(char *buffer, int start_index);
int get_opp_type(char * buffer, int *indx);
int check_exists(char * name_to_search, symPTR * root);
int mem_mode_info(char *buffer, int *info_arr);
int calc_base_addr(int addr);
int update_symbole(symPTR *manager, int ic, void * new_data, int data_type);
int parse2_file_stage_1(char * file_name, int *IC, int *DC, symPTR * root, mem_mode_node** mmn_root);
int parse2_file_stage_2(char * file_name, int *IC, int *DC,  int EXPORT_FILES, symPTR * root, mem_mode_node** mmn_root);

char * num_to_bin_str(int number);
char * find_symbol(char * buffer, int *stopped);
char * find_symbol(char *buffer, int *stopped);