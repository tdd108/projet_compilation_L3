#ifndef EXPR_TRANS
#define EXPR_TRANS

#include <stdbool.h>
#include "table_symbol.h"

int codeGenerator(Node *ast, HashTable global, FunctionTable *funcTables);

void parseVariableGlobalCodeGen(Node *globalDeclar, HashTable global, FILE *file);

void parseDeclarationsCodeGen(Node *declarations, HashTable variableTable, FILE *file);

void parseDeclarationCodeGen(Node *declarations, HashTable variableTable, FILE *file);

void variableCodeGen(Node *variable, HashTable variableTable, FILE *file);

void parseFunctionsCodeGen(Node *functions, HashTable globalVar, FunctionTable *funcTables, FILE *file);

void parseFunctionCodeGen(Node *functions, HashTable globalVar, FunctionTable *funcTables, FILE *file);

void parseParameterCodeGen(Node *parameters, int numParams, HashTable globalVar, HashTable localVar, FILE *file);

void functionCodeGen(Node *function, HashTable globalVar, HashTable localVar, FILE *file);

void parseInstructionsCodeGen(Node *instructions, HashTable globalVar, HashTable localVar, FILE *file, bool isMain);

void instructionCodeGen(Node *instr, HashTable globalVar, HashTable localVar, FILE *file, bool isMain);

void assignInstrCodeGen(Node *instr, HashTable globalVar, HashTable localVar, FILE *file, bool isMain);

void ifInstrCodeGen(Node *instr, HashTable globalVar, HashTable localVar, FILE *file, bool isMain);

void identInstrCodeGen(Node *instr, HashTable globalVar, HashTable localVar, FILE *file, bool isMain);

void whileInstrCodeGen(Node *instr, HashTable globalVar, HashTable localVar, FILE *file, bool isMain);

void returnInstrCodeGen(Node *expr, HashTable globalVar, HashTable localVar, FILE *file, bool isMain);

void parseExpressionCodeGen(Node *expressions, HashTable globalVar, HashTable localVar, FILE *file);

void expressionCodeGen(Node *expressions, HashTable globalVar, HashTable localVar, FILE *file);

void endMain(FILE *file);

void identExpr(Node *expr, HashTable globalVar, HashTable localVar, FILE *file);

void argumentCodeGen(Node *expr, int numArgs, HashTable globalVar, HashTable localVar, FILE *file);

void identFuncExpr(Node *expr, HashTable globalVar, HashTable localVar, FILE *file);

void identExprArray(Node *expr, HashTable globalVar, HashTable localVar, FILE *file);

void binOpExpr(Node *expr, HashTable globalVar, HashTable localVar, FILE *file);

void compOpExpr(Node *expr, HashTable globalVar, HashTable localVar, FILE *file);

void boolOpExpr(Node *expr, HashTable globalVar, HashTable localVar, FILE *file);

#endif