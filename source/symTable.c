#include "../include/symTable.h"


symPtr create_symTable_node(){
    static int id=0;
    symPtr newSym = (symPtr) malloc(sizeof(symNode));
    newSym->id = id;
    id++;
    newSym->offset = -1;
    newSym->value = -1;
    newSym->baseAddress = -1;
    newSym->symbol = NULL;
    newSym->next = NULL;
    for(int i=0; i<N;i++)
        newSym->arr[i] = 0;
    return newSym;
}

void insert_symTable(symPtr* root, symPtr node){
    if(*root==NULL)
        (*root)=node;
    else{
        symPtr temp = *root;
        while(temp->next)
            temp = (symPtr)temp->next;
        temp->next = (struct symNode *) node;
    }
}

void insertData_symTable(symPtr node, char *sym,int value, int baseAddress ,int offset , int arr[N]){
    node->offset = offset;
    node->value = value;
    node->baseAddress =baseAddress;
    node->symbol = sym;
    node->next = NULL;
    for(int i=0; i<N;i++)
        node->arr[i] = arr[i];
}

void printToFile_symTable(symPtr* root){
    FILE * fp = fopen("symTable.txt", "w");
    if(fp==NULL){
        printf("error in open file!\n");
        return;
    }
    char* buffer;
    char str[5];
    int i;
    symPtr temp = *root;
    while(temp){
        buffer= malloc(sizeof(char)*128);
        strcpy(buffer, temp->symbol);
        sprintf(str, "%d", temp->value);
        strcat(buffer, "\t");
        strcat(buffer, str);
        sprintf(str, "%d", temp->baseAddress);
        strcat(buffer, "\t");
        strcat(buffer, str);
        sprintf(str, "%d", temp->offset);
        strcat(buffer, "\t");
        strcat(buffer, str);
        strcat(buffer,"\t");
        for(i=0;i<N; i++){
            if(temp->arr[i]){
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
            }
        }
        strcat(buffer, "\n");
        fputs(buffer, fp);
        free(buffer);
    }
    fclose(fp);
}
void destroy(symPtr root);