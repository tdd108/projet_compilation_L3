#include <stdlib.h>
#include <stdio.h>

#include "semantic_analyzer.h"

int checkVarExpression(Node *instr, HashTable parameters, HashTable variablesLocal, HashTable globalVariable) {
    if (instr->label == Ident) {
        if (lookup(parameters, instr->value) == NULL && 
            lookup(variablesLocal, instr->value) == NULL &&
            lookup(globalVariable, instr->value) == NULL)
            printf("Variable not declared: %s\n", instr->value);
    }
    return 0;
}