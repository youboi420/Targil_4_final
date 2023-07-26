#ifndef TARGIL_4_SYMTABLE_H
#define TARGIL_4_SYMTABLE_H
#define N 4
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
    switch (i) {
        case 0:
            strcat(buffer, "external, ");
            break;
        case 1:
            strcat(buffer, "entry, ");
            break;
        case 2:
            strcat(buffer, "data, ");
            break;
        case 3:
            strcat(buffer, "code, ");
            break;
    }
*/

typedef struct symNode{
    int arr[N];
    int START_IC, END_IC;
    int id;
    int value;
    int baseAddress;
    int offset;
    char * symbol;
    struct symNode* next;
}symNode, *symPTR;

symPTR create_symTable_node();
void update_sym_addr(symPTR node, int baseAddress ,int offset);
void insert_symTable(symPTR* root, symPTR node);
void insertData_symTable(symPTR node, char *sym,int value, int baseAddress ,int offset , int arr[N], int START_IC, int END_IC);
void printToFile_symTable(symPTR* root);
void destroy_symTable(symPTR *root);
void print_sym_list(symPTR *root);
#endif //TARGIL_4_SYMTABLE_H