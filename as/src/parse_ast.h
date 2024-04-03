#ifndef PARSE_AST
#define PARSE_AST

#include "table_symbol.h"
#include "semantic_analyzer.h"

HashTable parseVariableGlobal(Node *globalDeclar);

void parseExpression(Node *instructions, HashTable parameters, HashTable variablesLocal, HashTable variablesGlobal);

void parseInstruction(Node *instructions, HashTable parameters, HashTable variablesLocal, HashTable variablesGlobal);

void parseInstructionRaw(Node *instr, HashTable parameters, HashTable variablesLocal, HashTable variablesGlobal);

FunctionTable *parseAst(Node *functionDeclaration, HashTable variablesGlobal);

#endif