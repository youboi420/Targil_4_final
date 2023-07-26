typedef struct entNode{
    int IC_INFO, DC_INFO;
    unsigned int baseAddress;
    unsigned int offset;
    char * symbol;
    struct entNode* next;
} entNode, *entPTR;

entPTR create_entTable_node();
void insert_entTable(entPTR* root, entPTR node);
void insertData_entTable(entPTR node, char *sym, int baseAddress ,int offset);
void printToFile_entTable(entPTR* root, char * file_name);
void destroy_entTable(entPTR *root);
void print_ent_list(entPTR *root);