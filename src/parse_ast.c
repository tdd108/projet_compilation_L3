#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse_ast.h"

unsigned int base_address = 0;

void parseAst(Node *ast, HashTable *globalDeclar, FunctionTable **functionDeclar) {
    if (ast == NULL) return;
    if (globalDeclar == NULL) return;
    if (functionDeclar == NULL) return;

    checkAndAddIdentifier(globalDeclar, makeSymbolFunction("getint", "int", 0, NULL, 0, base_address));
    base_address += 8;
    checkAndAddIdentifier(globalDeclar, makeSymbolFunction("getchar", "char", 0, NULL, 0, base_address));
    base_address += 8;
    ParamType params[] = {{INT}};
    checkAndAddIdentifier(globalDeclar, makeSymbolFunction("putint", "", 1, params, SIZE_INT, base_address));
    base_address += 8;
    params[0].type = CHAR;
    checkAndAddIdentifier(globalDeclar, makeSymbolFunction("putchar", "", 1, params, SIZE_CHAR, base_address));
    base_address += 8;

    parseVariableGlobal(FIRSTCHILD(ast), globalDeclar);

    parseFunctions(SECONDCHILD(ast), globalDeclar, functionDeclar);

    if (lookup(*globalDeclar, "main") == NULL)
        error(ast, MISSING_MAIN, SEMANTIC_ERR);
}

void parseVariableGlobal(Node *globalDeclar, HashTable *globalVarTable) {
    if (globalDeclar == NULL) return;
    if (globalVarTable == NULL) return;

    parseDeclarations(globalDeclar, globalVarTable, &base_address, SCOPE_GLOBAL);
}

void parseDeclarations(Node *declarations, HashTable *variableTable, unsigned int *baseAddress, Scope scope) {
    if (declarations == NULL) return;
    if (variableTable == NULL) return;

    Node *declaration = FIRSTCHILD(declarations);

    while (declaration != NULL) {
        parseDeclaration(declaration, variableTable, baseAddress, scope);
        declaration = declaration->nextSibling;
    }
}

void parseDeclaration(Node *declarations, HashTable *variableTable, unsigned int *baseAddress, Scope scope) {
    if (declarations == NULL) return;
    if (variableTable == NULL) return;

    char *type = declarations->name;
    Node *variable = FIRSTCHILD(declarations);
     

    while (variable != NULL) {
        variableEval(variable, type, variableTable, baseAddress, scope);
        variable = variable->nextSibling;
    }
}

void variableEval(Node *variable, char *type, HashTable *variableTable, unsigned int *baseAddress, Scope scope) {
    if (variable == NULL) return;
    if (type == NULL) return;
    if (variableTable == NULL) return;

    Symbol newSym;

    switch (variable->label) {
    case Ident:
        newSym = makeSymbolIdent(variable->ident, type, *baseAddress, scope);
        newSym.address += newSym.size;
        if (!checkAndAddIdentifier(variableTable, newSym))
            error(variable, EXISTED_TOKEN, SEMANTIC_ERR);
        *baseAddress += newSym.size;
        break;
    case Array:;
        int size = SECONDCHILD(variable)->intValue;
        if (size == 0) {
            error(FIRSTCHILD(variable), WRONG_TYPE_INDEX, SEMANTIC_ERR);
        }
        char *name = FIRSTCHILD(variable)->ident;
        newSym = makeSymbolArray(name, type, size, 1, *baseAddress, scope);
        newSym.address += newSym.arrType.baseType == INT ? 4 : 1;
        if (!checkAndAddIdentifier(variableTable, newSym))
            error(FIRSTCHILD(variable), EXISTED_TOKEN, SEMANTIC_ERR);
        *baseAddress += newSym.size;
        break;
    default:
        break;
    }
}

void parseFunctions(Node *functions, HashTable *varGlobal, FunctionTable **lstFuncTab) {
    if (functions == NULL) return;
    if (varGlobal == NULL) return;
    if (lstFuncTab == NULL) return;

    parseFunction(functions, varGlobal, lstFuncTab);
}

void parseFunction(Node *functions, HashTable *varGlobal, FunctionTable **lstFuncTab) {
    if (functions == NULL) return;
    if (varGlobal == NULL) return;
    if (lstFuncTab == NULL) return;

    Node *function = FIRSTCHILD(functions);

    while (function != NULL) {
        FunctionTable *functionTable = (FunctionTable*)malloc(sizeof(FunctionTable));
        if (!functionTable) {
            fprintf(stderr, "Memory allocation error\n");
            exit(EXIT_FAILURE);
        }

        functionEval(function, varGlobal, functionTable);

        functionTable->next = *lstFuncTab;
        functionTable->prev = NULL;
        if (*lstFuncTab != NULL)
           (*lstFuncTab)->prev = functionTable;
        *lstFuncTab = functionTable;

        function = function->nextSibling;
    }
}

void parseParameter(Node *parameters, int *numParameters, ParamType param[], HashTable *variableTable, unsigned int *baseAddress) {
    if (parameters == NULL) return;
    if (FIRSTCHILD(parameters)->label == Void) return;

    Node *parameter = FIRSTCHILD(parameters);
    Symbol newSym;
    while (parameter != NULL) {
        switch (parameter->label) {
        case Type:
            newSym = makeSymbolIdent(FIRSTCHILD(parameter)->ident, parameter->name, *baseAddress, SCOPE_LOCAL);
            newSym.address += newSym.size;
            param[*numParameters].type = newSym.type;
            if (!checkAndAddIdentifier(variableTable, newSym))
                error(FIRSTCHILD(parameter), EXISTED_TOKEN, SEMANTIC_ERR);
            *baseAddress += newSym.size;
            break;
        case Array:
            newSym = makeSymbolArray(SECONDCHILD(parameter)->ident, FIRSTCHILD(parameter)->name, 0, 0, *baseAddress, SCOPE_LOCAL); // use 0 as dimension to represent address, maybe stupid
            newSym.size = 8; // address
            newSym.address += newSym.size;
            param[*numParameters].type = ARRAY;
            param[*numParameters].arrType.dimension = newSym.arrType.dimension;
            param[*numParameters].arrType.baseType = newSym.arrType.baseType;
            if (!checkAndAddIdentifier(variableTable, newSym))
                error(SECONDCHILD(parameter), EXISTED_TOKEN, SEMANTIC_ERR);
            *baseAddress += newSym.size;
            break;
        default: break;
        }
        (*numParameters)++;
        parameter = parameter->nextSibling;
    }
    
}

void functionEval(Node *function, HashTable *varGlobal, FunctionTable *funcTab) {
    Node *header = FIRSTCHILD(function),
    *param = THIRDCHILD(header),
    *body = SECONDCHILD(function),
    *localVar = FIRSTCHILD(body), *instructions;
    Symbol newSym;
    char *funcType = FIRSTCHILD(header)->name,
    *funcName = SECONDCHILD(header)->ident;
    instructions = SECONDCHILD(body);

    funcTab->localVars = newHashTable();

    int numParameters = 0;
    unsigned int baseAddress = 0;
    ParamType params[MAX_PARAM_SIZE];
    parseParameter(param, &numParameters, params, &(funcTab->localVars), &baseAddress);
    parseDeclarations(localVar, &(funcTab->localVars), &baseAddress, SCOPE_LOCAL);

    newSym = makeSymbolFunction(funcName, funcType, numParameters, params, baseAddress, base_address);

    if (strcmp(funcName, "main") == 0 && newSym.funcType.returnType != INT)
        error(SECONDCHILD(header), WRONG_TYPE_FUNCTION, SEMANTIC_ERR);

    if (!checkAndAddIdentifier(varGlobal, newSym))
        error(SECONDCHILD(header), EXISTED_TOKEN, SEMANTIC_ERR);

    parseInstructions(instructions, funcTab->localVars, *varGlobal, newSym.funcType.returnType);

    base_address+= newSym.size;
}

void parseInstructions(Node *instructions, HashTable variablesLocal, HashTable variablesGlobal, type_t funcType) {
    if (instructions == NULL) return;

    Node *instruction = FIRSTCHILD(instructions);

    while (instruction != NULL) {
        parseInstruction(instruction, variablesLocal, variablesGlobal, funcType);
        instruction = instruction->nextSibling;
    }
}

void parseInstruction(Node *instr, HashTable varLocal, HashTable varGlobal, type_t funcType) {
    if (instr == NULL) return;

    switch (instr->label) {
        case Assign:
            assignInstr(instr, varLocal, varGlobal, funcType);
            break;
        case Ident:
            identInstr(instr, varLocal, varGlobal, funcType);
            break;
        case Return:
            returnInstr(instr, varLocal, varGlobal, funcType);
            break;
        case If:
            ifInstr(instr, varLocal, varGlobal, funcType);
            break;
        case While:
            whileInstr(instr, varLocal, varGlobal, funcType);
            break;
        case Instructions:
            parseInstructions(FIRSTCHILD(instr), varLocal, varGlobal, funcType);
            break;
        default: 
            break;
    }
}

void parseExpression(Node *expressions, HashTable varLocal, HashTable varGlobal) {
    if (expressions == NULL) return;

    Node *expr = FIRSTCHILD(expressions);

    while (expr != NULL) {
        parseExpression(expr, varLocal, varGlobal);
        expr = expr->nextSibling;
    }

    exprTypeEval(expressions, varLocal, varGlobal);
}

void assignInstr(Node *instr, HashTable varLocal, HashTable varGlobal, type_t funcType) {
    if (instr == NULL) return;

    Node *lvalue = FIRSTCHILD(instr), *rvalue = SECONDCHILD(instr);
    Symbol *tmp;
    
    // evaluate lvalue
    switch (lvalue->label) {
        case Ident:
            tmp = lookupAll(varLocal, varGlobal, lvalue->ident);
            if (tmp == NULL)
                error(lvalue, UNDEFINED_TOKEN, SEMANTIC_ERR);
            else if (tmp->type == ARRAY)
                error(lvalue, UNMODIFIABLE_LVALUE, SEMANTIC_ERR);
            else if (tmp->type == FUNCTION) 
                error(lvalue, UNMODIFIABLE_LVALUE, SEMANTIC_ERR);
            break;
        case Array:
            tmp = lookupAll(varLocal, varGlobal, FIRSTCHILD(lvalue)->ident);
        
            if (tmp == NULL)
                error(FIRSTCHILD(lvalue), UNDEFINED_TOKEN, SEMANTIC_ERR);

            parseExpression(SECONDCHILD(lvalue), varLocal, varGlobal);
            break;
        default:
            break;
    }
    exprTypeEval(lvalue, varLocal, varGlobal);

    // evaluate rvalue
    if (rvalue != NULL) 
        parseExpression(rvalue, varLocal, varGlobal);

    // check assign type
    if (lvalue->type == CHAR && rvalue->type == INT)
        warning(lvalue, ASSIGN_INT_TO_CHAR);
    else if (rvalue->type == VOID_T) {
        if (lvalue->label == Array)
            error(FIRSTCHILD(lvalue), ASSIGN_TO_VOID_FUNCTION, SEMANTIC_ERR);
        error(lvalue, ASSIGN_TO_VOID_FUNCTION, SEMANTIC_ERR);
    }
    else if (rvalue->type == ARRAY)
        error(rvalue, UNDEFINED_ERROR, SEMANTIC_ERR);
}

void returnInstr(Node *instr, HashTable varLocal, HashTable varGlobal, type_t funcType) {
    if (instr == NULL) 
        return;

    if (FIRSTCHILD(instr) == NULL)
        return;

    parseExpression(FIRSTCHILD(instr), varLocal, varGlobal);

    switch (FIRSTCHILD(instr)->type) {
    case VOID_T:
        error(FIRSTCHILD(instr), WRONG_TYPE_RETURN, SEMANTIC_ERR);
        break;
    case ARRAY:
        error(FIRSTCHILD(instr), WRONG_TYPE_RETURN, SEMANTIC_ERR);
        break;
    case INT:
        if (funcType == CHAR)
            warning(FIRSTCHILD(instr), ASSIGN_INT_TO_CHAR);
        break;
    default:
        break;
    }

    if (funcType == VOID_T)
        error(FIRSTCHILD(instr), WRONG_TYPE_RETURN, SEMANTIC_ERR);

    // if (FIRSTCHILD(instr)->type == VOID_T)
    //     error(FIRSTCHILD(instr), WRONG_TYPE_RETURN, SEMANTIC_ERR);

    // if (FIRSTCHILD(instr)->type == ARRAY)
    //     error(FIRSTCHILD(instr), WRONG_TYPE_RETURN, SEMANTIC_ERR);

    // if (FIRSTCHILD(instr)->type == INT && funcType == CHAR)
    //     warning(FIRSTCHILD(instr), ASSIGN_INT_TO_CHAR);
}

void identInstr(Node *instr, HashTable varLocal, HashTable varGlobal, type_t funcType) {
    if (instr == NULL) return;

    parseExpression(instr, varLocal, varGlobal);
}

void ifInstr(Node *instr, HashTable varLocal, HashTable varGlobal, type_t funcType) {
    if (instr == NULL)
        return;

    parseExpression(FIRSTCHILD(instr), varLocal, varGlobal);

    if (FIRSTCHILD(instr)->type == VOID_T)
        error(FIRSTCHILD(instr), INVALID_TYPE_TOKEN, SEMANTIC_ERR);

    parseInstruction(SECONDCHILD(instr), varLocal, varGlobal, funcType);
    
    if (THIRDCHILD(instr) != NULL)
        parseInstruction(THIRDCHILD(instr), varLocal, varGlobal, funcType);
}

void whileInstr(Node *instr, HashTable varLocal, HashTable varGlobal, type_t funcType) {
    if (instr == NULL)
        return;

    parseExpression(FIRSTCHILD(instr), varLocal, varGlobal);

    if (FIRSTCHILD(instr)->type == VOID_T)
        error(FIRSTCHILD(instr), INVALID_TYPE_TOKEN, SEMANTIC_ERR);

    parseInstruction(SECONDCHILD(instr), varLocal, varGlobal, funcType);
}