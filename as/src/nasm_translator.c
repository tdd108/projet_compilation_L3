#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nasm_translator.h"

void nasmTranslator() {}

void expressionTranslator(Node *expr) {
    FILE *nasmFile = fopen("./_anonymous.asm", "a");
    char op[5] = "";
    switch(expr->label) {
        case AddSub:
            if (strcmp(expr->value, "-") == 0) strcpy(op, "sub"); else strcpy(op, "add");
            addSub(nasmFile, op, expr);
            break;
        case DivStar:
            if (strcmp(expr->value, "/") == 0) strcpy(op, "idiv"); else strcpy(op, "imul");
            divStar(nasmFile, op, expr);
            break;
        case Order:
            orderTranslator(nasmFile, expr);
            break;
        case num: case Character:
            valueTranslator(nasmFile, expr);
            break;
        case Ident:
            accessMemory(nasmFile, expr);
            break;
        default:
        break;
    }
    fclose(nasmFile);
}

// void instructionTranslator(Node *instr) {
//     FILE *nasmFile = fopen("./_anonymous.asm", "a");
//     switch(instr->label) {
//         case Assign:
//             assign(nasmFile, instr);
//             break;
//         default:
//         break;
//     }
//     fclose(nasmFile);
// }

void writeMain() {
    FILE *nasmFile = fopen("./_anonymous.asm", "a");
    fprintf(nasmFile, 
    "section .text\n"
    "global _start\n"
    "_start:\n"
    );
    fclose(nasmFile);
}

void writeEndMain() {
    FILE *nasmFile = fopen("./_anonymous.asm", "a");
    fprintf(nasmFile, 
    ";finish\n"
    "mov rax, 60\n"
    "mov rdi, 0\n"
    "syscall\n"
    );
    fclose(nasmFile);
}

void initStaticMemory(char *name) {
    FILE *nasmFile = fopen("./_anonymous.asm", "a");
    fprintf(nasmFile,
    // TODO: don't really understand what resq does, so gonna put qword for now.
    // "%s: resq 0\n", name
    "%s: dq 0\n", name
    );
    fclose(nasmFile);
}

void assign(Node *instr) {
    FILE *nasmFile = fopen("./_anonymous.asm", "a");
    fprintf(nasmFile, "pop qword [%s]\n", FIRSTCHILD(instr)->value);
    fclose(nasmFile);
}

void accessMemory(FILE *nasmFile, Node *expr) {
    fprintf(nasmFile, "push qword [%s]\n", expr->value);
}

void valueTranslator(FILE *nasmFile, Node *expr) {
    fprintf(nasmFile, 
    "push %s\n", expr->value
    );
}

void addSub(FILE *nasmFile, char *op, Node *expr) {
    fprintf(nasmFile, ";addsub\n");

    fprintf(nasmFile,
    "pop rcx\n"
    "pop rax\n"
    "%s rax, rcx\n"
    "push rax\n", op
    );
}

void divStar(FILE *nasmFile, char *op, Node *expr) {
    fprintf(nasmFile, ";divstar\n");
    
    if (strcmp(op, "imul") == 0)
        fprintf(nasmFile,
        "pop rcx\n"
        "pop rax\n"
        "imul rax, rcx\n"
        "push rax\n"
        );
    else if (strcmp(op, "idiv") == 0)
        fprintf(nasmFile,
        "mov rdx, 0\n"
        "pop rcx\n"
        "pop rax\n"
        "idiv rcx\n"
        "push rax\n"
        );
}

void orderTranslator(FILE *nasmFile, Node *expr) {
    fprintf(nasmFile,
    "; order\n"
    "pop rax\n"
    "pop rcx\n"
    "cmp rax, rcx\n"
    );
}

void startIf(Node *instr, int nb_if, int nb_else) {
    FILE *nasmFile = fopen("./_anonymous.asm", "a");
    char op[5] = "";
    if (strcmp(FIRSTCHILD(instr)->value, ">") == 0)
            strcpy(op, "jg");
    else if (strcmp(FIRSTCHILD(instr)->value, "<") == 0)
            strcpy(op, "jl");
    else if (strcmp(FIRSTCHILD(instr)->value, ">=") == 0)
            strcpy(op, "jge");
    else if (strcmp(FIRSTCHILD(instr)->value, "<=") == 0)
            strcpy(op, "jle");
    else if (strcmp(FIRSTCHILD(instr)->value, "==") == 0)
            strcpy(op, "je");
    
    if (THIRDCHILD(instr) == NULL)
        fprintf(nasmFile,
        "%s endif%d\n", op, nb_if
        );
    else
        fprintf(nasmFile,
        "%s else%d\n", op, nb_if
        );
    fclose(nasmFile);
}

void elseIf(Node *instr, int nb_if, int nb_else) {
    FILE *nasmFile = fopen("./_anonymous.asm", "a");
    fprintf(nasmFile,
    "jmp endif%d\n"
    "else%d:\n", nb_if, nb_else
    );
    fclose(nasmFile);
}

void endIf(Node *instr, int nb_if) {
    FILE *nasmFile = fopen("./_anonymous.asm", "a");
    fprintf(nasmFile,
    "endif%d:\n", nb_if
    );
    fclose(nasmFile);
}