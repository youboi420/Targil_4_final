typedef struct extNode{
    int IC_INFO, DC_INFO;
    unsigned int baseAddress;
    unsigned int offset;
    char * symbol;
    struct extNode* next;
} extNode, *extPTR;

extPTR create_extTable_node();
void insert_extTable(extPTR* root, extPTR node);
void insertData_extTable(extPTR node, char *sym, int baseAddress ,int offset);
void printToFile_extTable(extPTR* root, char * file_name);
void destroy_extTable(extPTR *root);
void print_ext_list(extPTR *root);