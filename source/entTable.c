#include "../include/entTable.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

entPTR create_entTable_node(){
    entPTR nNode = (entPTR)malloc(sizeof(entNode));
    nNode->offset = -1;
    nNode->baseAddress = -1;
    nNode->symbol = NULL;
    nNode->next = NULL;
    return nNode;
}

void insert_entTable(entPTR* root, entPTR node){
    if(*root==NULL)
        (*root)=node;
    else{
        entPTR temp = *root;
        while(temp->next)
            temp = (entPTR)temp->next;
        temp->next = (struct entNode *) node;
    }
}

void insertData_entTable(entPTR node, char *sym, int baseAddress ,int offset){
    node->offset = offset;
    node->baseAddress =baseAddress;
    node->symbol = sym;
    node->next = NULL;
}

void printToFile_entTable(entPTR* root, char * file_name){
    char * buffer, nName[strlen(file_name) + 5];
    char str[10];
    entPTR temp = *root;
    FILE *fp;
    strcat(nName, ".ent");
    fp = fopen(nName, "w+");

    while(temp){
        buffer = (char *)malloc(sizeof(char) * 128 * 2);        
        strcpy(buffer, temp->symbol);
        strcat(buffer, ",");
        sprintf(str, "%d", temp->baseAddress);
        strcat(buffer, str);
        strcat(buffer, ",");
        sprintf(str, "%d", (temp->offset - temp->baseAddress)); /* needs to be like MAIN,96,4  for example the main label which came first is the first addres in memory*/
        strcat(buffer, "\n");
        fputs(buffer, fp);
        temp = temp->next;
        free(buffer);
    }
}

void destroy_entTable(entPTR *root){
    entPTR temp = *root, save;
    while(temp){
        free(temp->symbol);
        save = temp->next;
        free(temp);
        temp = save;
    }
}