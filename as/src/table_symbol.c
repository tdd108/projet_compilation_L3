#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "table_symbol.h"

// hashValue << 5 Left-shifting is equivalent to multiplying by 2 
// raised to the power of the number of shifted bits (that's new :v)
// multiplication by 32 is a convenient way to increase the "weight" of the 
// current character in the hash value, contributing to the uniqueness of the resulting hash value 
// (don't know what does it mean but just keep it here for the moment)
unsigned int hash(char *key) {
    unsigned int hashValue = 0;
    while (*key) {
        hashValue = (hashValue << 5) + *key++;
    }
    return hashValue % HASH_TABLE_SIZE;
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
    strcpy(newNode->symbol.name, symbol.name);
    strcpy(newNode->symbol.type, symbol.type);

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

void checkAndAddIdentifier(HashTable* hashTable, char* name, char *type) {
    Symbol* existingSymbol = lookup(*hashTable, name);
    if (!existingSymbol) {
        Symbol newSymbol;
        strcpy(newSymbol.name, name);
        strcpy(newSymbol.type, type);
        insert(hashTable, newSymbol);
    }
}

HashTable createTableFromNode(Node *declaration) {
    HashTable table = {0};

    if (declaration == NULL) 
        return table;

    Node *type = declaration->firstChild;
    while (type != NULL) {
        Node *variable = type->firstChild;
        while (variable != NULL) {
            checkAndAddIdentifier(&table, variable->value, type->value);
            variable = variable->nextSibling;
        }
        type = type->nextSibling;
    }

    return table;
}

void setAddressGlobalTable(HashTable global) {
    unsigned int base = 0;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* current = global.nodes[i];
        while (current != NULL) {
            current->symbol.address = base;
            base += 8;
            current = current->next;
        }
    }   
}

HashTable populateGlobalSymbolTable(Node *globalDeclaration) {
    HashTable global = createTableFromNode(globalDeclaration);
    setAddressGlobalTable(global);
    return global;
}

void printTable(HashTable table) {
    printf("+------------+---------------+-------------+\n");
    printf("|   Name     |     Type      |   Address   |\n");
    printf("+------------+---------------+-------------+\n");
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* current = table.nodes[i];
        while (current != NULL) {
            printf("| %-10s | %-13s | 0x%-8X |\n", current->symbol.name, current->symbol.type, current->symbol.address);
            current = current->next;
        }
    }   
    printf("+------------+---------------+-------------+\n");
}

void printLstTable(FunctionTable *lstTable) {
    FunctionTable *current = lstTable;
    while (current != NULL) {
        printf("Parameters Table\n");
        printTable(current->parameters);
        printf("Local Variables Table\n");
        printTable(current->variablesLocal);
        current = current->next;
    }
}