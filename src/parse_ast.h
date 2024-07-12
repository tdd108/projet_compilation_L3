#ifndef PARSE_AST
#define PARSE_AST

#include "table_symbol.h"
#include "semantic_analyzer.h"
#include "error.h"

void parseAst(Node *ast, HashTable *globalVarTable, FunctionTable **funcTable);

void parseVariableGlobal(Node *globalDeclar, HashTable *globalVarTable);

void parseDeclarations(Node *declarations, HashTable *variableTable, unsigned int *baseAddress, Scope scope);

void parseDeclaration(Node *declaration, HashTable *variableTable, unsigned int *baseAddress, Scope scope);

void variableEval(Node *variable, char *type, HashTable *variableTable, unsigned int *baseAddress, Scope scope);

void parseFunctions(Node *functions, HashTable *varGlobal, FunctionTable **lstFuncTab);

void parseFunction(Node *functions, HashTable *varGlobal, FunctionTable **lstFuncTab);

void parseParameter(Node *parameters, int *numParameters, ParamType *param, HashTable *variableTable, unsigned int *baseAddress);

void functionEval(Node *function, HashTable *varGlobal, FunctionTable *funcTab);

void parseInstructions(Node *instructions, HashTable varLocal, HashTable varGlobal, type_t funcType);

void parseInstruction(Node *instr, HashTable varLocal, HashTable varGlobal, type_t funcType);

void parseExpression(Node *expressions, HashTable variablesLocal, HashTable variablesGlobal);

void assignInstr(Node *instr, HashTable varLocal, HashTable varGlobal, type_t funcType);

void ifInstr(Node *instr, HashTable varLocal, HashTable varGlobal, type_t funcType);

void whileInstr(Node *instr, HashTable varLocal, HashTable varGlobal, type_t funcType);

void returnInstr(Node *instr, HashTable varLocal, HashTable varGlobal, type_t funcType);

void identInstr(Node *instr, HashTable varLocal, HashTable varGlobal, type_t funcType);

#endif