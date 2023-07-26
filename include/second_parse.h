#include "../include/my_error.h"
#include "entTable.h"
#include "extTable.h"
#include "symTable.h"


int parse2_file_stage_1(char * file_name, int *IC, int *DC, symPTR * root);
int parse2_file_stage_2(char * file_name, int *IC, int *DC,  int EXPORT_FILES, symPTR * root);
void read_to_symTable(symPTR node);
int update_symbole(symPTR *manager, int ic, void * new_data, int data_type);
void create_ob_file( symPTR *manager, char * file_name);
void create_ent_file( entPTR *manager, char * file_name);
void create_ext_file( extPTR *manager, char * file_name);
int check_arrtib(char *buffer, int index);
char * find_symbol(char *buffer, int *stopped);
int check_R_E(char * buffer, int *label_index);
int check_exists(char * name_to_search, symPTR * root);