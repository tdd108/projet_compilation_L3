#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "code_generator.h"

int numLabel = 0;

void newLabel(char label[]) {
    sprintf(label, "label_%d", numLabel);
    numLabel++;
}

bool isBooleanExpr(Node *expr) {
    return expr->label == Eq || expr->label == Order || 
        expr->label == And || expr->label == Or || expr->label == Not;
}

void my_putchar(FILE *file) {
    fprintf(file,
    "putchar:\n"
    "push rbp\n"
    "mov rbp, rsp\n"
    "push rdi\n"
    "mov rax, 1\n"
    "mov rdi, 1\n"
    "mov rsi, rsp\n"
    "mov rdx, 1\n"
    "syscall\n"
    "mov rsp, rbp\n"
    "pop rbp\n"
    "ret\n"
    );
}

void my_getchar(FILE *file) {
    fprintf(file,
    "getchar:\n"
    "push rbp\n"
    "mov rbp, rsp\n"
    "sub rsp, 8\n"
    "mov rax, 0\n"
    "mov rdi, 0\n"
    "mov rsi, rsp\n"
    "mov rdx, 1\n"
    "syscall\n"
    "pop rax\n"
    "mov rsp, rbp\n"
    "pop rbp\n"
    "ret\n"
    );
}

void my_putint(FILE *file) {
    fprintf(file,
    "putint:\n"
    "push rbp\n"
    "mov rbp, rsp\n"
    "mov r9, 11\n"
    "sub rsp, 4\n"
    "mov dword [rbp - 4], edi\n"
    "cmp dword [rbp - 4], 0\n"
    "jne putint_loop_start0\n"
    "mov rdi, 48\n"
    "call putchar\n"
    "jmp putint_loop_end_2\n"
    "putint_loop_start0:\n"
    "putint_loop_start:\n"
    "cmp dword [rbp - 4], 0\n"
    "jle putint_loop_end\n"
    "mov rdx, 0\n"
    "mov eax, dword [rbp - 4]\n"
    "mov rbx, 10\n"
    "idiv rbx\n"
    "mov dword [rbp - 4], eax\n"
    "add rdx, 48\n"
    "push rdx\n"
    "dec r9\n"
    "jmp putint_loop_start\n"
    "putint_loop_end:\n"
    "add r9, 1\n"
    "putint_loop_start_2:\n"
    "cmp r9, 11\n"
    "jg putint_loop_end_2\n"
    "mov rdi, 0\n"
    "pop rdi\n"
    "call putchar\n"
    "inc r9\n"
    "jmp putint_loop_start_2\n"
    "putint_loop_end_2:\n"
    "mov rdi, 10\n"
    "call putchar\n"
    "mov rsp, rbp\n"
    "pop rbp\n"
    "ret\n"
    );
}

void my_getint(FILE *file) {
    fprintf(file,
    "getint:\n"
    "push rbp\n"
    "mov rbp, rsp\n"
    "sub rsp, 4\n"
    "mov dword [rbp - 4], 0\n"
    "my_getint_start:\n"
    "call getchar\n"
    "cmp al, 48\n"
    "jl my_getint_end\n"
    "cmp al, 57\n"
    "jg my_getint_end\n"
    "sub al, 48\n"
    "cmp dword [rbp - 4], 0\n"
    "je equal\n"
    "mov rbx, 0\n"
    "mov ebx, dword [rbp - 4]\n"
    "imul ebx, 10\n"
    "mov dword [rbp - 4], ebx\n"
    "add byte [rbp - 4], al\n"
    "jmp end\n"
    "equal:\n"
    "mov byte [rbp - 4], al\n"
    "end:\n"
    "jmp my_getint_start\n"
    "my_getint_end:\n"
    "mov eax, dword [rbp - 4]\n"
    "mov rsp, rbp\n"
    "pop rbp\n"
    "ret\n"
    );
}

int codeGenerator(Node *ast, HashTable global, FunctionTable *funcTables) {
    FILE *file = fopen("./_anonymous.asm", "w");
    file = fopen("./_anonymous.asm", "w");
    if (file == NULL) {
        printf("failed to open file\n");
        return 0;
    }

    parseVariableGlobalCodeGen(FIRSTCHILD(ast), global, file);

    parseFunctionsCodeGen(SECONDCHILD(ast), global, funcTables, file);
    
    fclose(file);
    return 1;
}

void parseVariableGlobalCodeGen(Node *globalDeclar, HashTable global, FILE *file) {
    if (globalDeclar == NULL) return;
    if (file == NULL) return;

    fprintf(file, "section .bss\n");

    parseDeclarationsCodeGen(globalDeclar, global, file);
}

void parseDeclarationsCodeGen(Node *declarations, HashTable variableTable, FILE *file) {
    if (declarations == NULL) return;
    if (file == NULL) return;

    Node *declaration = FIRSTCHILD(declarations);

    while (declaration != NULL) {
        parseDeclarationCodeGen(declaration, variableTable, file);
        declaration = declaration->nextSibling;
    }
}

void parseDeclarationCodeGen(Node *declarations, HashTable variableTable, FILE *file) {
    if (declarations == NULL) return;
    if (file == NULL) return;

    Node *variable = FIRSTCHILD(declarations);

    while (variable != NULL) {
        variableCodeGen(variable, variableTable, file);
        variable = variable->nextSibling;
    }
}

void variableCodeGen(Node *variable, HashTable variableTable, FILE *file) {
    if (variable == NULL) return;
    if (file == NULL) return;

    Symbol *tmp;

    switch (variable->label) {
    case Ident:
        tmp = lookup(variableTable, variable->ident);
        if (tmp == NULL) break;
        if (tmp->type == INT)
            fprintf(file, "%s: resd 1\n", tmp->name);
        if (tmp->type == CHAR)
            fprintf(file, "%s: resb 1\n", tmp->name);
        break;
    case Array:;
        tmp = lookup(variableTable, FIRSTCHILD(variable)->ident);
        if (tmp == NULL) break;
        if (tmp->arrType.baseType == INT)
            fprintf(file, "%s: resd %d\n", tmp->name, tmp->size / SIZE_INT);
        if (tmp->arrType.baseType == CHAR)
            fprintf(file, "%s: resb %d\n", tmp->name, tmp->size);
        break;
    default:
        break;
    }
}

void parseFunctionsCodeGen(Node *functions, HashTable globalVar, FunctionTable *funcTables, FILE *file) {
    if (functions == NULL) return;
    if (file == NULL) return;

    fprintf(file, 
    "section .text\n"
    "global _start\n"
    );
    
    parseFunctionCodeGen(functions, globalVar, funcTables, file);

    my_putchar(file);
    my_getchar(file);
    my_putint(file);
    my_getint(file);
}

void parseFunctionCodeGen(Node *functions, HashTable globalVar, FunctionTable *funcTables, FILE *file) {
    if (functions == NULL) return;
    if (file == NULL) return;

    Node *function = FIRSTCHILD(functions);

    // reverse the list of symbol table of functions
    FunctionTable *funcTable = funcTables;
    while (funcTable->next != NULL) {
        funcTable = funcTable->next;
    }

    while (function != NULL) {
        functionCodeGen(function, globalVar, funcTable->localVars, file);

        funcTable = funcTable->prev;
        function = function->nextSibling;
    }
    endMain(file);
}

void parseParameterCodeGen(Node *parameters, int numParams, HashTable globalVar, HashTable localVar, FILE *file) {
    if (parameters == NULL) return;;
    if (file == NULL) return;

    Node *param = FIRSTCHILD(parameters);
    if (param->label == Void) return;

    if (numParams == 0)
        return;

    char registes[6][5] = {"r9", "r8", "rcx", "rdx", "rsi", "rdi"};
    char *ident, *size, *sizeReg;
    Symbol *paramSymbol;
    while (param != NULL) {
        switch (param->label) {
            case Type:
                ident = FIRSTCHILD(param)->ident;
                paramSymbol = lookupAll(localVar, globalVar, ident);
                if (paramSymbol == NULL) return;
                break;
            case Array:
                ident = SECONDCHILD(param)->ident;
                paramSymbol = lookupAll(localVar, globalVar, ident);
                if (paramSymbol == NULL) return;
                break;
            default:
                break;
        }
        switch (paramSymbol->type) {
            case INT:
                size = "dword";
                sizeReg = "eax";
                break;
            case CHAR:
                size = "byte";
                sizeReg = "al";
                break;
            case ARRAY:
                size = "qword";
                sizeReg = "rax";
                break;
            default:
                break;
        }
        if (numParams <= 6) {
            fprintf(file,
            "mov rax, %s\n"
            "mov %s [rbp - %d], %s\n",
            registes[6 - numParams], size, paramSymbol->address, sizeReg
            );
        }
        else {
            fprintf(file,
            "mov %s, %s [rbp + %d]\n"
            "mov %s [rbp - %d], %s\n",
            sizeReg, size, 8 * (numParams - 5),
            size, paramSymbol->address, sizeReg
            );
        }
        param = param->nextSibling;
        numParams--;
    }
}

void functionCodeGen(Node *function, HashTable globalVar, HashTable localVar, FILE *file) {
    if (function == NULL) return;
    if (file == NULL) return;

    Symbol *tmp;

    Node *header = FIRSTCHILD(function),
    *body = SECONDCHILD(function),
    *instructions = SECONDCHILD(body);

    char *funcName = SECONDCHILD(header)->ident;

    tmp = lookup(globalVar, funcName);

    fprintf(file, 
    "%s:\n"
    "push rbp\n"
    "mov rbp, rsp\n"
    "sub rsp, %d\n"
    , tmp->name, tmp->funcType.sizeMem);

    //? parameters here?
    parseParameterCodeGen(THIRDCHILD(header), tmp->funcType.numParameters, globalVar, localVar, file);
    //? declaration here?
    bool isMain = strcmp(funcName, "main") == 0;
    parseInstructionsCodeGen(instructions, globalVar, localVar, file, isMain);

    if (isMain) {
        fprintf(file, "mov rdi, 0\n");
    }

    fprintf(file,
    "mov rsp, rbp\n"
    "pop rbp\n"
    "ret\n");
}   

void parseInstructionsCodeGen(Node *instructions, HashTable globalVar, HashTable localVar, FILE *file, bool isMain) {
    if (instructions == NULL) return;
    if (file == NULL) return;

    if (instructions->label != Instructions) {
        instructionCodeGen(instructions, globalVar, localVar, file, isMain);
        return;
    }

    Node *instruction = FIRSTCHILD(instructions);

    while (instruction != NULL) {
        instructionCodeGen(instruction, globalVar, localVar, file, isMain);
        instruction = instruction->nextSibling;
    }
}

void instructionCodeGen(Node *instr, HashTable globalVar, HashTable localVar, FILE *file, bool isMain) {
    if (instr == NULL) return;
    if (file == NULL) return;

    switch (instr->label) {
        case Assign:
            assignInstrCodeGen(instr, globalVar, localVar, file, isMain);
            break;
        case Ident:
            identInstrCodeGen(instr, globalVar, localVar, file, isMain);
            break;
        case Return:
            returnInstrCodeGen(instr, globalVar, localVar, file, isMain);
            break;
        case If:
            ifInstrCodeGen(instr, globalVar, localVar, file, isMain);
            break;
        case While:
            whileInstrCodeGen(instr, globalVar, localVar, file, isMain);
            break;
        case Instructions:
            parseInstructionsCodeGen(FIRSTCHILD(instr), globalVar, localVar, file, isMain);
            break;
        default: 
            break;
    }
}

void lvalueCodeGen(Node *lvalue, HashTable globalVar, HashTable localVar, FILE *file) {
    if (lvalue == NULL) return;
    if (file == NULL) return;

    Symbol *symbol;
    char *size, *sizeReg;
    switch (lvalue->label) {
        case Ident:
            symbol = lookupAll(localVar, globalVar, lvalue->ident);
            if (symbol == NULL) return;
            
            size = symbol->type == INT ? "dword" : "byte";
            sizeReg = symbol->type == INT ? "eax" : "al";
            fprintf(file, "pop rax\n");
            if (symbol->scope == SCOPE_GLOBAL)
                fprintf(file, "mov %s [%s], %s\n", size, lvalue->ident, sizeReg);
            else {
                fprintf(file, "mov %s [rbp - %d], %s\n", size, symbol->address, sizeReg);
            }
            break;
        case Array:
            symbol = lookupAll(localVar, globalVar, FIRSTCHILD(lvalue)->ident);
            if (symbol == NULL) return;

            expressionCodeGen(SECONDCHILD(lvalue), globalVar, localVar, file); // index

            int typeSize = symbol->arrType.baseType == INT ? 4 : 1;
            size = symbol->arrType.baseType == INT ? "dword" : "byte";
            sizeReg = symbol->arrType.baseType == INT ? "eax" : "al";

            switch (symbol->scope) {
                case SCOPE_GLOBAL:
                    fprintf(file, "mov rbx, %s\n", symbol->name);
                    break;
                case SCOPE_LOCAL:
                    if (symbol->arrType.dimension == 0) {
                        fprintf(file, 
                        "mov rbx, qword [rbp - %d]\n"
                        , symbol->address);
                    }
                    else {
                        fprintf(file, 
                        "mov rbx, rbp\n"
                        "sub rbx, %d\n"
                        , symbol->address);
                    }
                    break;
                default:
                    break;
            }   
            fprintf(file,
            "mov rax, %d\n"
            "pop rdx\n"
            "imul rax, rdx\n"
            "%s rbx, rax\n"
            "pop rax\n"
            "mov %s [rbx], %s\n"
            , typeSize, symbol->scope == SCOPE_GLOBAL ? "add":"sub", size, sizeReg
            );
        default:
            break;
    }
}

void rvalueCodeGen(Node *rvalue, HashTable globalVar, HashTable localVar, FILE *file) {
    if (rvalue == NULL) return;
    if (file == NULL) return;

    if (isBooleanExpr(rvalue)) {
        char trueLabel[10], falseLabel[10], endLabel[10];

        newLabel(trueLabel);
        newLabel(falseLabel);
        newLabel(endLabel);

        strcpy(rvalue->true_t, trueLabel);
        strcpy(rvalue->false_t, falseLabel);

        boolOpExpr(rvalue, globalVar, localVar, file);

        fprintf(file, 
        "%s:\n"
        "push 0\n"
        "jmp %s\n", falseLabel, endLabel);
        
        fprintf(file, "%s:\n", trueLabel);

        fprintf(file, 
        "push 1\n");

        fprintf(file, "%s:\n", endLabel);
    }
    else {
        expressionCodeGen(rvalue, globalVar, localVar, file);
    }
}

void assignInstrCodeGen(Node *instr, HashTable globalVar, HashTable localVar, FILE *file, bool isMain) {
    if (instr == NULL) return;
    if (file == NULL) return;

    Node *lvalue = FIRSTCHILD(instr);
    Node *rvalue = SECONDCHILD(instr);
    if (lvalue == NULL || rvalue == NULL) return;
    
    rvalueCodeGen(rvalue, globalVar, localVar, file);

    lvalueCodeGen(lvalue, globalVar, localVar, file);
}

void ifCodeGen(Node *instr, HashTable globalVar, HashTable localVar, FILE *file, bool isMain) {
    if (instr == NULL) return;
    if (file == NULL) return;

    char ifTrue[10], ifAfter[10];
    newLabel(ifTrue);
    newLabel(ifAfter);

    strcpy(FIRSTCHILD(instr)->true_t, ifTrue);
    strcpy(FIRSTCHILD(instr)->false_t, ifAfter);
    boolOpExpr(FIRSTCHILD(instr), globalVar, localVar, file);

    strcpy(SECONDCHILD(instr)->false_t, ifAfter);
    fprintf(file, "%s:\n", ifTrue);
    parseInstructionsCodeGen(SECONDCHILD(instr), globalVar, localVar, file, isMain);

    fprintf(file, "%s:\n", SECONDCHILD(instr)->false_t);
}

void ifElseCodeGen(Node *instr, HashTable globalVar, HashTable localVar, FILE *file, bool isMain) {
    if (instr == NULL) return;
    if (file == NULL) return;

    char ifTrue[10], ifFalse[10], ifElseAfter[10];
    newLabel(ifTrue);
    newLabel(ifFalse);
    newLabel(ifElseAfter);

    strcpy(FIRSTCHILD(instr)->true_t, ifTrue);
    strcpy(FIRSTCHILD(instr)->false_t, ifFalse);

    boolOpExpr(FIRSTCHILD(instr), globalVar, localVar, file);

    strcpy(SECONDCHILD(instr)->false_t, ifElseAfter);
    fprintf(file, "%s:\n", ifTrue);
    parseInstructionsCodeGen(SECONDCHILD(instr), globalVar, localVar, file, isMain);
    fprintf(file, "jmp %s\n", ifElseAfter);

    strcpy(THIRDCHILD(instr)->false_t, ifElseAfter);
    fprintf(file, "%s:\n", ifFalse);
    parseInstructionsCodeGen(THIRDCHILD(instr), globalVar, localVar, file, isMain);
    fprintf(file, "%s:\n", ifElseAfter);
}

void ifInstrCodeGen(Node *instr, HashTable globalVar, HashTable localVar, FILE *file, bool isMain) {
    if (instr == NULL) return;
    if (file == NULL) return;

    if (THIRDCHILD(instr) == NULL) {
        ifCodeGen(instr, globalVar, localVar, file, isMain);
    }
    else  {
        ifElseCodeGen(instr, globalVar, localVar, file, isMain);
    }
}

void whileInstrCodeGen(Node *instr, HashTable globalVar, HashTable localVar, FILE *file, bool isMain) {
    if (instr == NULL) return;
    if (file == NULL) return;

    char begin[10], iftrue[10];
    newLabel(instr->false_t);
    newLabel(begin);
    newLabel(iftrue);

    strcpy(FIRSTCHILD(instr)->true_t, iftrue);
    strcpy(FIRSTCHILD(instr)->false_t, instr->false_t);

    fprintf(file, "%s:\n", begin);

    boolOpExpr(FIRSTCHILD(instr), globalVar, localVar, file);

    strcpy(SECONDCHILD(instr)->false_t, begin);

    fprintf(file, "%s:\n", iftrue);

    parseInstructionsCodeGen(SECONDCHILD(instr), globalVar, localVar, file, isMain);

    fprintf(file, "jmp %s\n", begin);

    fprintf(file, "%s:\n", instr->false_t);
}

void returnInstrCodeGen(Node *instr, HashTable globalVar, HashTable localVar, FILE *file, bool isMain) {
    char trueLabel[10], falseLabel[10], endLabel[10];

    if (FIRSTCHILD(instr) != NULL) {
        if (isBooleanExpr(FIRSTCHILD(instr))) {
            newLabel(trueLabel);
            newLabel(falseLabel);
            newLabel(endLabel);
            strcpy(FIRSTCHILD(instr)->true_t, trueLabel);
            strcpy(FIRSTCHILD(instr)->false_t, falseLabel);
        }
        
        expressionCodeGen(FIRSTCHILD(instr), globalVar, localVar, file);

        if (isBooleanExpr(FIRSTCHILD(instr))) {
            fprintf(file, 
            "%s:\n"
            "push 1\n"
            "jmp %s\n"
            "%s:\n"
            "push 0\n"
            "%s:\n"
            , FIRSTCHILD(instr)->true_t, endLabel, FIRSTCHILD(instr)->false_t, endLabel);
        }

        if (!isMain)
            fprintf(file, "pop rax\n");
        else 
            fprintf(file, "pop rdi\n");
    }

    fprintf(file,
    "mov rsp, rbp\n"
    "pop rbp\n"
    "ret\n"
    );
}

void identInstrCodeGen(Node *instr, HashTable globalVar, HashTable localVar, FILE *file, bool isMain) {
    if (instr == NULL) return;
    if (file == NULL) return;

    Symbol *tmp = lookup(globalVar, instr->ident);
    if (tmp->type != FUNCTION) return;

    argumentCodeGen(FIRSTCHILD(instr), tmp->funcType.numParameters, globalVar, localVar, file);
    fprintf(file, "call %s\n", instr->ident);
}

void expressionCodeGen(Node *expr, HashTable globalVar, HashTable localVar, FILE *file) {
    if (expr == NULL) return;
    if (file == NULL) return;

    switch(expr->label) {
        case num:
            fprintf(file, "push %d\n", expr->intValue);
            break;
        case Character:
            if (strcmp(expr->charValue, "'\\n'") == 0) {
                fprintf(file, "push 10\n");
                break;
            }
            if (strcmp(expr->charValue, "'\\t'") == 0) {
                fprintf(file, "push 9\n");
                break;
            }
            fprintf(file, "push %s\n", expr->charValue);
            break;
        case AddSub: case DivStar:
            binOpExpr(expr, globalVar, localVar, file);
            break;
        case Order: case Eq: case And: case Or: case Not:
            boolOpExpr(expr, globalVar, localVar, file);
            break;
        case Ident:
            identExpr(expr, globalVar, localVar, file);
            break;
        case Array:
            identExprArray(expr, globalVar, localVar, file);
            break;
        default:
        break;
    }
}

void identExpr(Node *expr, HashTable globalVar, HashTable localVar, FILE *file) {
    if (expr == NULL) return;
    if (file == NULL) return;

    Symbol *symbol = lookupAll(localVar, globalVar, expr->ident);
    if (symbol == NULL) return;

    char *size, *sizeReg;
    switch (symbol->type) {
        case INT: case CHAR:
            size = symbol->type == INT ? "dword" : "byte";
            sizeReg = symbol->type == INT ? "eax" : "al";
            switch (symbol->scope) {
                case SCOPE_GLOBAL:
                    fprintf(file,
                    "mov rax, 0\n"
                    "mov %s, %s [%s]\n"
                    "push rax\n", sizeReg, size, symbol->name);
                    break;
                case SCOPE_LOCAL:
                    fprintf(file,
                    "mov rax, 0\n"
                    "mov %s, %s [rbp - %d]\n"
                    "push rax\n", sizeReg, size, symbol->address);
                    break;
                default:
                    break;
            }
            break;
        case FUNCTION:
            identFuncExpr(expr, globalVar, localVar, file);
            break;
        case ARRAY: // this case is for when an array is passed as a parameters
            switch (symbol->scope) {
                case SCOPE_GLOBAL:
                    fprintf(file,"push %s\n",symbol->name);
                    break;
                case SCOPE_LOCAL:
                    if (symbol->arrType.dimension == 0) { // if array is a address
                       fprintf(file,
                        "mov rax, qword [rbp - %d]\n"
                        "push rax\n"
                        , symbol->address
                        );
                    }
                    else {
                        fprintf(file,
                        "mov rax, rbp\n"
                        "sub rax, %d\n"
                        "push rax\n"
                        , symbol->address
                        );
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void argumentCodeGen(Node *expr, int numArgs, HashTable globalVar, HashTable localVar, FILE *file) {
    if (expr == NULL) return;
    if (file == NULL) return;

    Node *arg = FIRSTCHILD(expr);

    if (numArgs == 0)
        return;

    char *registes[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    while (arg != NULL) {
        expressionCodeGen(arg, globalVar, localVar, file);

        if (numArgs <= 6) {
            fprintf(file, "pop %s\n", registes[numArgs - 1]);
        }

        arg = arg->nextSibling;
        numArgs--;
    }
}

void identFuncExpr(Node *expr, HashTable globalVar, HashTable localVar, FILE *file) {
    if (expr == NULL) return;
    if (file == NULL) return;

    Symbol *symbol = lookup(globalVar, expr->ident);
    if (symbol == NULL || symbol->type != FUNCTION) return;

    argumentCodeGen(FIRSTCHILD(expr), symbol->funcType.numParameters, globalVar, localVar, file);

    fprintf(file, "call %s\n", expr->ident);

    if (symbol->funcType.returnType != VOID_T)
        fprintf(file, "push rax\n");
}

void identExprArray(Node *expr, HashTable globalVar, HashTable localVar, FILE *file) {
    if (expr == NULL) return;
    if (file == NULL) return;

    Symbol *symbol = lookupAll(localVar, globalVar, FIRSTCHILD(expr)->ident);
    if (symbol == NULL) return;

    expressionCodeGen(SECONDCHILD(expr), globalVar, localVar, file);

    int typeSize = symbol->arrType.baseType == INT ? 4 : 1;
    char *size, *sizeReg;
    size = symbol->arrType.baseType == INT ? "dword" : "byte";
    sizeReg = symbol->arrType.baseType == INT ? "eax" : "al";

    switch (symbol->scope) {
        case SCOPE_GLOBAL:
            fprintf(file, "mov rbx, %s\n", symbol->name);
            break;
        case SCOPE_LOCAL:
            if (symbol->arrType.dimension == 0) {
                fprintf(file, "mov rbx, qword [rbp - %d]\n", symbol->address);
            }
            else {
                fprintf(file, 
                "mov rbx, rbp\n"
                "sub rbx, %d\n"
                , symbol->address);
            }
            break;
        default:
            break;
    }
    
    fprintf(file,
	"mov rax, %d\n"
    "pop rdx\n"
    "imul rax, rdx\n"
    "%s rbx, rax\n"
    "mov rax, 0\n"
    "mov %s, %s [rbx]\n"
    "push rax\n"
    , typeSize, symbol->scope == SCOPE_GLOBAL ? "add":"sub", sizeReg, size
    );
}

void binOpExpr(Node *expr, HashTable globalVar, HashTable localVar, FILE *file) {
    if (expr == NULL) return;
    if (file == NULL) return;

    expressionCodeGen(FIRSTCHILD(expr), globalVar, localVar, file);
    expressionCodeGen(SECONDCHILD(expr), globalVar, localVar, file);

    switch (expr->bin_op) {
        case '+': case '-':
            fprintf(file,
            "pop rcx\n"
            "pop rax\n"
            "%s rax, rcx\n"
            "push rax\n",
            expr->bin_op == '+' ? "add" : "sub");
            break;
        case '*':
            fprintf(file,
            "pop rcx\n"
            "pop rax\n"
            "imul rax, rcx\n"
            "push rax\n");
            break;
        case '/':
            fprintf(file,
            "mov rdx, 0\n"
            "pop rcx\n"
            "pop rax\n"
            "idiv rcx\n"
            "push rax\n"
            );
            break;
        case '%':
            fprintf(file,
            "mov rdx, 0\n"
            "pop rcx\n"
            "pop rax\n"
            "idiv rcx\n"
            "push rdx\n"
            );
            break;
        default:
            break;
    }
}

void compOpExpr(Node *expr, HashTable globalVar, HashTable localVar, FILE *file) {
    if (expr == NULL) return;
    if (file == NULL) return;

    char comOp[4] = "j";

    if (expr->comp_op[0] == '<') comOp[1] = 'l';
    else if (expr->comp_op[0] == '>') comOp[1] = 'g';
    else if (expr->comp_op[0] == '=') comOp[1] = 'e';
    else if (expr->comp_op[0] == '!') comOp[1] = 'n';

    if (expr->comp_op[0] != '=' && expr->comp_op[1] == '=') 
        comOp[2] = 'e';
    comOp[3] = '\0';

    char endLabel1[10], endLabel2[10];
    if (isBooleanExpr(FIRSTCHILD(expr))) {
        newLabel(FIRSTCHILD(expr)->true_t);
        newLabel(FIRSTCHILD(expr)->false_t);
        newLabel(endLabel1);
    }
    expressionCodeGen(FIRSTCHILD(expr), globalVar, localVar, file);
    if (isBooleanExpr(FIRSTCHILD(expr))) {
        fprintf(file, 
        "%s:\n"
        "push 1\n"
        "jmp %s\n"
        "%s:\n"
        "push 0\n"
        "%s:\n"
        , FIRSTCHILD(expr)->true_t, endLabel1, FIRSTCHILD(expr)->false_t, endLabel1);
    }

    if (isBooleanExpr(SECONDCHILD(expr))) {
        newLabel(SECONDCHILD(expr)->true_t);
        newLabel(SECONDCHILD(expr)->false_t);
        newLabel(endLabel2);
    }
    expressionCodeGen(SECONDCHILD(expr), globalVar, localVar, file);
    if (isBooleanExpr(SECONDCHILD(expr))) {
        fprintf(file, 
        "%s:\n"
        "push 1\n"
        "jmp %s\n"
        "%s:\n"
        "push 0\n"
        "%s:\n"
        , SECONDCHILD(expr)->true_t, endLabel2, SECONDCHILD(expr)->false_t, endLabel2);
    }

    fprintf(file,
    "pop rcx\n"
    "pop rax\n"
    "cmp rax, rcx\n"
    "%s %s\n"
    "jmp %s\n", comOp, expr->true_t, expr->false_t
    );
}

void boolOpExpr(Node *expr, HashTable globalVar, HashTable localVar, FILE *file) {
    char tmp[10];

    switch (expr->label) {
        case Or:
            strcpy(FIRSTCHILD(expr)->true_t, expr->true_t);
            newLabel(tmp);
            strcpy(FIRSTCHILD(expr)->false_t, tmp);
            boolOpExpr(FIRSTCHILD(expr), globalVar, localVar, file);
            fprintf(file, "%s:\n", tmp);

            strcpy(SECONDCHILD(expr)->true_t, expr->true_t);
            strcpy(SECONDCHILD(expr)->false_t, expr->false_t);
            boolOpExpr(SECONDCHILD(expr), globalVar, localVar, file);
            break;
        
        case And:
            strcpy(FIRSTCHILD(expr)->true_t, expr->true_t);
            newLabel(tmp);
            strcpy(FIRSTCHILD(expr)->true_t, tmp);
            strcpy(FIRSTCHILD(expr)->false_t, expr->false_t);
            boolOpExpr(FIRSTCHILD(expr), globalVar, localVar, file);
            fprintf(file, "%s:\n", tmp);

            strcpy(SECONDCHILD(expr)->true_t, expr->true_t);
            strcpy(SECONDCHILD(expr)->false_t, expr->false_t);
            boolOpExpr(SECONDCHILD(expr), globalVar, localVar, file);
            break;

        case Not:
            strcpy(FIRSTCHILD(expr)->true_t, expr->false_t);
            strcpy(FIRSTCHILD(expr)->false_t, expr->true_t);
            boolOpExpr(FIRSTCHILD(expr), globalVar, localVar, file);
            break;

        case Eq: case Order:
            compOpExpr(expr, globalVar, localVar, file);
            break;

        default:
            expressionCodeGen(expr, globalVar, localVar, file);
            if (expr->label != Order && expr->label != Eq) 
                fprintf(file,
                "push 0\n"
                "pop rax\n"
                "pop rcx\n"
                "cmp rax, rcx\n"
                "jne %s\n"
                "jmp %s\n", expr->true_t, expr->false_t
                );
            break;
    }
}

void endMain(FILE *file) {
    if (file == NULL) return;

    fprintf(file,
    "_start:\n"
    "call main\n"
    "mov rax, 60\n"
    "syscall\n"
    );
}