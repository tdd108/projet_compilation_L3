#ifndef TAB_SYM_H
#define TAB_SYM_H

#define HASH_TABLE_SIZE 100
#define MAX_NAME_SIZE 50
#define MAX_TYPE_SIZE 20

#include "nasm_translator.h"
#include "tree.h"

typedef struct symbol{
    char name[MAX_NAME_SIZE];
    char type[MAX_TYPE_SIZE];
    int size; //byte
    int dimension;
    unsigned int address;
} Symbol;

typedef struct HashNode {
    Symbol symbol;
    struct HashNode* next;
} HashNode;

typedef struct HashTable {
    HashNode *nodes[HASH_TABLE_SIZE];
} HashTable;

typedef struct FunctionTable {
    HashTable parameters;
    HashTable variablesLocal;
    struct FunctionTable* next;
} FunctionTable;

unsigned int hash(char* key);

void insert(HashTable* hashTable, Symbol symbol);

Symbol* lookup(HashTable hashTable, char* name);

void checkAndAddIdentifier(HashTable* hashTable, char* name, char *type);

HashTable createTableFromNode(Node *declaration);

void setAddressGlobalTable(HashTable global);

HashTable populateGlobalSymbolTable(Node *globalDeclaration);

void printTable(HashTable table);

void printLstTable(FunctionTable *lstTable);

#endif