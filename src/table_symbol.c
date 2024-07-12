#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "table_symbol.h"

unsigned int hash(char *key) {
    unsigned int hashValue = 0;
    while (*key) {
        hashValue = (hashValue << 5) + *key++;
    }
    return hashValue % HASH_TABLE_SIZE;
}

HashTable newHashTable() {
    HashTable new = {0};
    return new;
}

Symbol makeSymbol(char* name, type_t type, char *base, int size, int dimenson, int numParameters, ParamType parameters[], unsigned int address, int sizeMem, Scope scope) {
    Symbol newSymbol;
    type_t baseType;

    strcpy(newSymbol.name, name);
    newSymbol.address = address;
    newSymbol.type = type;
    newSymbol.scope = scope;

    if (base != NULL) {
        baseType = strcmp(base, "int") == 0 ? INT : CHAR;
        if (strcmp(base, "int") == 0)
            baseType = INT;
        else if (strcmp(base, "char") == 0)
            baseType = CHAR;
        else if (strcmp(base, "") == 0)
            baseType = VOID_T;
    }

    switch (type) {
        case INT:
            newSymbol.size = size * SIZE_INT;
            break;
        case CHAR:
            newSymbol.size = size * SIZE_CHAR;
            break;
        case ARRAY:
            newSymbol.size = size * (baseType == INT ? SIZE_INT : SIZE_CHAR);
            newSymbol.arrType.dimension = dimenson;
            newSymbol.arrType.baseType = baseType;
            break;
        case FUNCTION:
            newSymbol.size = size;
            newSymbol.funcType.numParameters = numParameters;
            for (int i = 0; i < numParameters; i++)
                newSymbol.funcType.parameters[i] = parameters[i];
            newSymbol.funcType.returnType = baseType;
            newSymbol.funcType.sizeMem = sizeMem;
            break;
        default:
            break;
    }
        
    return newSymbol;
}

Symbol makeSymbolIdent(char* name, char *type, unsigned int address, Scope scope) {
    type_t baseType = strcmp(type, "int") == 0 ? INT : CHAR;
    return makeSymbol(name, baseType, NULL, 1, 0, 0, NULL, address, 0, scope);
}

Symbol makeSymbolArray(char* name, char *base, int size, int dimension, unsigned int address, Scope scope) {
    return makeSymbol(name, ARRAY, base, size, dimension, 0, NULL, address, 0, scope);
}

Symbol makeSymbolFunction(char *name, char *base, int numParameters, ParamType parameters[], int sizeMem, unsigned int address) {
    return makeSymbol(name, FUNCTION, base, 1, 0, numParameters, parameters, address, sizeMem, SCOPE_GLOBAL);
}

void insert(HashTable* hashTable, Symbol symbol) {
    unsigned int index = hash(symbol.name);

    // Create a new node
    HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));
    if (!newNode) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    // Copy the symbol information
    newNode->symbol = symbol;

    // Insert at the beginning of the linked list at the hash index
    newNode->next = hashTable->nodes[index];
    hashTable->nodes[index] = newNode;
}

Symbol* lookup(HashTable hashTable, char* name) {
    unsigned int index = hash(name);

    // Traverse the linked list at the hash index
    HashNode* current = hashTable.nodes[index];
    while (current != NULL) {
        if (strcmp(current->symbol.name, name) == 0) {
            return &current->symbol; // Found the symbol
        }
        current = current->next;
    }

    return NULL; // Symbol not found
}

Symbol *lookupAll(HashTable local, HashTable global, char* name) {
    Symbol *tmp = NULL;
    tmp = lookup(local, name);
    if (tmp != NULL) return tmp;
    return lookup(global, name);
}

int checkAndAddIdentifier(HashTable* hashTable, Symbol newSymbol) {
    Symbol* existingSymbol = lookup(*hashTable, newSymbol.name);
    if (!existingSymbol) {
        insert(hashTable, newSymbol);
        return 1;
    }
    return 0;
}

void printTable(HashTable table) {
    printf("+---------------+---------------+---------------+---------------+\n");
    printf("|     Name      |     Type      |     Size      |    Address    |\n");
    printf("+---------------+---------------+---------------+---------------+\n");
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* current = table.nodes[i];
        char *type;
        while (current != NULL) {
            switch (current->symbol.type) {
            case INT:
                type = "int";
                break;
            case CHAR:
                type = "char";
                break;
            case ARRAY:
                type = "array";
                break;
            case FUNCTION:
                type = "function";
                break;
            default:
                type = "";
                break;
            }
            printf("| %-13s | %-13s | %-13d | %-13d |\n", 
                    current->symbol.name, type, current->symbol.size, current->symbol.address);
            current = current->next;
        }
    }   
    printf("+---------------+---------------+---------------+---------------+\n");
}

void printLstTable(FunctionTable *lstTable) {
    FunctionTable *current = lstTable;
    while (current != NULL) {
        printf("Local Variables Table\n");
        printTable(current->localVars);
        current = current->next;
    }
}