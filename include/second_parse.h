#include "../include/my_error.h"
#include "entTable.h"
#include "extTable.h"
#include "symTable.h"

typedef enum OPP_TYPE {type_reg = 200, type_num, type_ext, type_ent} OPP_TYPE;

typedef struct mem_mode_node{
    int location;
    int arr[2];
    struct mem_mode_node *next;
} mem_mode_node;

void print_mmn_list(mem_mode_node**root);
void create_ob_file( symPTR *manager, FILE *main_fp,char * file_name);
void read_to_symTable(symPTR node);
void ext_put_zero(char * node_name, symPTR *root);
void handle_data(int type_dot, FILE *helper_fp, char * buffer,int index, symPTR *root);

int get_opp_type(char * buffer, int *indx);
int mem_mode_info(char *buffer, int *info_arr);
int check_exists(char * name_to_search, symPTR * root);
int valid_line(char * buffer);
int calc_delta(int arr[2]);
int check_arrtib(char *buffer, int start_index);
int calc_base_addr(int addr);
int parse2_file_stage_1(char * file_name, int *IC, int *DC, symPTR * root, mem_mode_node** mmn_root);
int parse2_file_stage_2(char * file_name, int *IC, int *DC,  int EXPORT_FILES, symPTR * root);
int update_symbole(symPTR *manager, int ic, void * new_data, int data_type);
int check_R_E(char * buffer, int *label_index);
int check_exists(char * name_to_search, symPTR * root);
int check_arrtib(char *buffer, int index);

char * find_symbol(char * buffer, int *stopped);
char * find_symbol(char *buffer, int *stopped);
char * num_to_bin_str(int number);