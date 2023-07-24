#include "../include/my_error.h"
#include "entTable.h"
#include "extTable.h"
#include "symTable.h"

int parse2_file_stage_1(char * file_name, int *IC, int *DC);
int parse2_file_stage_2(char * file_name, int *IC, int *DC,  int EXPORT_FILES);
void read_to_symTable(symPTR node);
int update_symbole(symPTR *manager, int ic, void * new_data, int data_type);
void create_ob_file( symPTR *manager, char * file_name);
void create_ent_file( entPTR *manager, char * file_name);
void create_ext_file( extPTR *manager, char * file_name);