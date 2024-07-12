#ifndef TYPE_CONTROL_H
#define TYPE_CONTROL_H

#include "table_symbol.h"
#include "error.h"

void exprTypeEval(Node *expr, HashTable local, HashTable global);

void exprBinOp(Node *expr, HashTable local, HashTable global);

void exprArray(Node *expr, HashTable local, HashTable global);

void exprIdent(Node *expr, HashTable global, HashTable local);

int checkParam(int numArgs, ParamType params[], Node *arguments, HashTable local, HashTable global);

void exprCompOp(Node *expr, HashTable local, HashTable global);

void exprNot(Node *expr, HashTable local, HashTable global);

#endif