#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse_ast.h"

int nb_if = 0;
int nb_else = 0;

HashTable parseVariableGlobal(Node *globalDeclar) {
    FILE *nasmFile = fopen("./_anonymous.asm", "w");
    fprintf(nasmFile,
    "section .data\n"
    );
    fclose(nasmFile);

    HashTable global = populateGlobalSymbolTable(globalDeclar);
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* current = global.nodes[i];
        while (current != NULL) {
            initStaticMemory(current->symbol.name);
            current = current->next;
        }
    }
    return global;
}

void parseExpression(Node *expressions, HashTable parameters, HashTable variablesLocal, HashTable variablesGlobal) {
    Node *expr = NULL;
    if (expressions == NULL)
        return;

    expr = expressions->firstChild;
    while (expr != NULL) {
        checkVarExpression(expr, parameters, variablesLocal, variablesGlobal);
        parseExpression(expr, parameters, variablesLocal, variablesGlobal);
        expr = expr->nextSibling;
    }

    expressionTranslator(expressions);
}

void parseInstructionRaw(Node *instr, HashTable parameters, HashTable variablesLocal, HashTable variablesGlobal) {
    switch (instr->label) {
        case Assign:
            parseExpression(SECONDCHILD(instr), parameters, variablesLocal, variablesGlobal);
            assign(instr);
            break;
        case Instructions:
            parseInstruction(instr->firstChild, parameters, variablesLocal, variablesGlobal);
            break;
        case If:
            Node *tmp = instr;
            parseExpression(FIRSTCHILD(tmp), parameters, variablesLocal, variablesGlobal);
            startIf(tmp, nb_if, nb_else);
            
            if (SECONDCHILD(tmp)->label == Instructions)
                parseInstruction(SECONDCHILD(tmp)->firstChild, parameters, variablesLocal, variablesGlobal);
            else parseInstructionRaw(SECONDCHILD(tmp), parameters, variablesLocal, variablesGlobal);

            if (THIRDCHILD(tmp) != NULL)
                elseIf(tmp, nb_if, nb_else);
            
            parseInstruction(THIRDCHILD(tmp), parameters, variablesLocal, variablesGlobal); 
            
            if (THIRDCHILD(tmp) == NULL) {
                endIf(tmp, nb_if);
                nb_if++;
            }
            
            break;
        default: break;
    }
}

void parseInstruction(Node *instr, HashTable parameters, HashTable variablesLocal, HashTable variablesGlobal) {
    while (instr != NULL) {
        parseInstructionRaw(instr, parameters, variablesLocal, variablesGlobal);
        instr = instr->nextSibling;
    }
}

FunctionTable *parseAst(Node *functionDeclaration, HashTable variablesGlobal) {
    FunctionTable *lstFunctionTable = NULL;

    Node *function = functionDeclaration->firstChild;
    Node *header, *body, *param, *localVar, *instructions; 

    while (function != NULL) {
        FunctionTable *functionTable = (FunctionTable*)malloc(sizeof(FunctionTable));
        if (!functionTable) {
            fprintf(stderr, "Memory allocation error\n");
            exit(EXIT_FAILURE);
        }
        
        char *functionName = function->firstChild->firstChild->nextSibling->value;
        
        header = FIRSTCHILD(function);
        param = THIRDCHILD(header);
        functionTable->parameters = createTableFromNode(param);

        body = SECONDCHILD(function);
        localVar = FIRSTCHILD(body);
        functionTable->variablesLocal = createTableFromNode(localVar);
        //nasm edit
        if (strcmp(functionName, "main") == 0) {
            writeMain();
        }
        instructions = SECONDCHILD(body);
        parseInstruction(instructions->firstChild, functionTable->parameters, functionTable->variablesLocal, variablesGlobal);
        writeEndMain();
        //end edit

        functionTable->next = lstFunctionTable;
        lstFunctionTable = functionTable;

        function = function->nextSibling;
    }
    
    return lstFunctionTable;
}