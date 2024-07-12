#ifndef TAB_SYM_H
#define TAB_SYM_H

#define HASH_TABLE_SIZE 100
#define MAX_NAME_SIZE 50
#define MAX_TYPE_SIZE 20
#define MAX_PARAM_SIZE 20

#define SIZE_INT 4
#define SIZE_CHAR 1

#include "tree.h"
typedef struct {
    type_t baseType;
    int dimension;
} ArrayType;

typedef struct paramtype {
    type_t type;
    ArrayType arrType;
} ParamType;

typedef struct {
    type_t returnType;
    int numParameters;
    ParamType parameters[MAX_PARAM_SIZE];
    int sizeMem;
} FunctionType;

typedef enum {
    SCOPE_GLOBAL,
    SCOPE_LOCAL,
} Scope;

typedef struct symbol{
    char name[MAX_NAME_SIZE];
    type_t type;
    union {
        ArrayType arrType;
        FunctionType funcType;
    };
    Scope scope;
    int size;
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
    HashTable localVars;
    struct FunctionTable* prev;
    struct FunctionTable* next;
} FunctionTable;

unsigned int hash(char* key);

Symbol makeSymbol(char* name, type_t type, char *base, int size, int dimenson, int numParameters, ParamType parameters[], unsigned int address, int sizeMem, Scope scope);

Symbol makeSymbolIdent(char* name, char *type, unsigned int address, Scope scope);

Symbol makeSymbolArray(char* name, char *base, int size, int dimension, unsigned int address, Scope scope);

Symbol makeSymbolFunction(char *name, char *base, int numParameters, ParamType parameters[], int sizeMem, unsigned int address);

void insert(HashTable* hashTable, Symbol symbol);

Symbol* lookup(HashTable hashTable, char* name);

Symbol *lookupAll(HashTable local, HashTable global, char* name);

HashTable newHashTable();

int checkAndAddIdentifier(HashTable* hashTable, Symbol newSymbol);

void printTable(HashTable table);

void printLstTable(FunctionTable *lstTable);

#endif