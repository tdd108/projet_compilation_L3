#ifndef NASM_EDITOR_H
#define NASM_EDITOR_H

#include "utils.h"
#include "tree.h"

void nasmTranslator();

void writeMain();

void writeEndMain();

void expressionTranslator(Node *expr);

void instructionTranslator(Node *instr);

void valueTranslator(FILE *nasmFile, Node *expr);

void identTranslator(FILE *nasmFile, Node *expr);

void divStar(FILE *nasmFile, char *op, Node *expr);

void addSub(FILE *nasmFile, char *op, Node *expr);

void orderTranslator(FILE *nasmFile, Node *expr);

void startIf(Node *instr, int nb_if, int nb_else);

void elseIf(Node *instr, int nb_if, int nb_else);

void endIf(Node *instr, int nb_if);

void initStaticMemory(char *name);

void assign(Node *instr);

void accessMemory(FILE *nasmFile, Node *expr);

#endif