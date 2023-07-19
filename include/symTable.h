#ifndef TARGIL_4_SYMTABLE_H
#define TARGIL_4_SYMTABLE_H
#define N 4
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct symNode{
    int arr[N];
    int id;
    int value;
    int baseAddress;
    int offset;
    char * symbol;
    struct symNode* next;
}symNode, *symPtr;

symPtr create_symTable_node();
void insert_symTable(symPtr* root, symPtr node);
void insertData_symTable(symPtr node, char *sym,int value, int baseAddress ,int offset , int arr[N]);
void printToFile_symTable(symPtr* root);
void destroy_symTable(symPtr root);

#endif //TARGIL_4_SYMTABLE_H
