#include "../include/symTable.h"

symPTR create_symTable_node(){
    static int id=0;
    symPTR newSym = (symPTR) malloc(sizeof(symNode));
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

void insert_symTable(symPTR* root, symPTR node){
    if(*root==NULL){
        (*root)=node;
    }
    else{
        symPTR temp = *root;
        while(temp->next)
            temp = temp->next;
        temp->next = node;
    }
}

void print_sym_list(symPTR *root){
    char* buffer;
    char str[10];
    int index = 0;
    int i;
    symPTR temp = *root;
    while(temp){
        buffer = malloc(sizeof(char)*512);
        strcpy(buffer,"Symbol: ");
        strcat(buffer, temp->symbol);
        strcat(buffer, " value: ");
        sprintf(str, "%d", temp->value);
        strcat(buffer, str);
        strcat(buffer, " Base_Addres: ");
        sprintf(str, "%d", temp->baseAddress);
        strcat(buffer, " ");
        strcat(buffer, str);
        strcat(buffer, " Offset: ");
        sprintf(str, "%d", temp->offset);
        strcat(buffer, str);
        strcat(buffer, "\t");
        for(i=0;i<N; i++){
            // printf("ARR[%i] = %i\n", i, temp->arr[i]);
            if(temp->arr[i] == 1){
                if(i == 0) strcat(buffer, "external, ");
                if (i == 1) strcat(buffer, "entry, ");
                if (i == 2) strcat(buffer, "data, ");
                if (i == 3) strcat(buffer, "code, ");
            }
        }
        
        sprintf(str, "%d", temp->END_IC);
        strcat(buffer, "\tLEN:  ");
        strcat(buffer, str); 
        strcat(buffer, "\n");
        printf("%s\n", buffer);
        temp = temp->next;//NULL;
    }
}

void update_sym_addr(symPTR node, int baseAddress ,int offset){
    node->baseAddress = baseAddress;
    node->offset = offset;
}

void insertData_symTable(symPTR node, char *sym,int value, int baseAddress ,int offset , int arr[N], int START_IC, int END_IC){
    node->offset = offset;
    node->value = value;
    node->baseAddress =baseAddress;
    node->symbol = sym;
    for(int i=0; i<N;i++)
        node->arr[i] = arr[i];
    node->next = NULL;
}

void printToFile_symTable(symPTR* root){
    FILE * fp = fopen("symTable.txt", "w");
    if(fp==NULL){
        printf("error in open file!\n");
        return;
    }
    char* buffer;
    char str[10];
    int i;
    symPTR temp = *root;
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
        temp = temp->next;
    }
    fclose(fp);
}

void destroy_symTable(symPTR *root){
    symPTR temp = *root, save;
    while(temp){
        free(temp->symbol);
        save = temp->next;
        free(temp);
        temp = save;
    }
    *root = NULL;
}