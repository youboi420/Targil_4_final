#include "../include/extTable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extPTR create_extTable_node(){
    extPTR newExt_node = (extPTR)malloc(sizeof(extNode));
    newExt_node->offset = -1;
    newExt_node->baseAddress = -1;
    newExt_node->symbol = NULL;
    newExt_node->next = NULL;
    return newExt_node;
}

void insert_extTable(extPTR* root, extPTR node){
    if(*root==NULL)
        (*root)=node;
    else{
        extPTR temp = *root;
        while(temp->next)
            temp = (extPTR)temp->next;
        temp->next = (struct extNode *) node;
    }
}

void update_ext_addr(extPTR node, int baseAddress ,int offset){
    node->baseAddress = baseAddress;
    node->offset = offset;
}

void insertData_extTable(extPTR node, char *sym, int baseAddress ,int offset){
    node->offset = offset;
    node->baseAddress =baseAddress;
    node->symbol = sym;
    node->next = NULL;
}

void printToFile_extTable(extPTR* root,char * file_name){
    char * buffer, nName[80 + 5];
    char str[10];
    extPTR temp = *root;
    FILE *fp;
    strcpy(nName, file_name);
    strcat(nName, ".ext");
    fp = fopen(nName, "w+");

    while(temp){
        buffer = (char *)malloc(sizeof(char) * 128 * 2);        
        strcpy(buffer, temp->symbol);
        strcat(buffer, " ");
        strcat(buffer, "BASE ");
        sprintf(str, "%d", temp->baseAddress);
        strcat(buffer, str);
        strcat(buffer, "\n");

        strcat(buffer, temp->symbol);
        strcat(buffer, " ");
        strcat(buffer, "OFFSET ");
        sprintf(str, "%d", temp->offset);
        strcat(buffer, str);
        strcat(buffer, "\n");
        fputs(buffer, fp);
        temp = temp->next;
        free(buffer);
    }
    fclose(fp);
}

void print_ext_list(extPTR* root){
    char * buffer;
    char str[10];
    extPTR temp = *root;
    
    while(temp){
        buffer = (char *)malloc(sizeof(char) * 128 * 2);        
        strcpy(buffer, temp->symbol);
        strcat(buffer, " ");
        strcat(buffer, "BASE ");
        sprintf(str, "%d", temp->baseAddress);
        strcat(buffer, str);
        strcat(buffer, "\n");

        strcat(buffer, temp->symbol);
        strcat(buffer, " ");
        strcat(buffer, "OFFSET ");
        sprintf(str, "%d", temp->offset);
        strcat(buffer, str);
        strcat(buffer, "\n");
        printf("[ext]\n%s\n", buffer);
        temp = temp->next;
        free(buffer);
    }
}

void destroy_extTable(extPTR *root){
    extPTR temp = *root, save;
    while(temp){
        free(temp->symbol);
        save = temp->next;
        free(temp);
        temp = save;
    }
}