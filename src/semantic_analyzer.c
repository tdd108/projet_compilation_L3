#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "semantic_analyzer.h"


void exprTypeEval(Node *expr, HashTable local, HashTable global) {
    if (expr == NULL) return;

    switch (expr->label) {
        case Character:
            expr->type = CHAR;
            break;
        case num:
            expr->type = INT;
            break;
        case Ident:
            exprIdent(expr, local, global);
            break;
        case Array:
            exprArray(expr, local, global);
            break;
        case AddSub: case DivStar:
            exprBinOp(expr, local, global);
            break;
        case Eq:case Order:case And:case Or:
            exprCompOp(expr, local, global);
        case Not:
            exprNot(expr, local, global);
            break;
        default:
            break;
    }
}

void exprIdent(Node *expr, HashTable local, HashTable global) {
    if (expr == NULL) return;

    Symbol *tmp = lookupAll(local, global, expr->ident);
    if (tmp == NULL) error(expr, UNDEFINED_TOKEN, SEMANTIC_ERR);
    
    if (tmp->type != FUNCTION && FIRSTCHILD(expr) != NULL && FIRSTCHILD(expr)->label == Arguments)
        error(expr, NOT_FUNCTION, SEMANTIC_ERR);

    // if ident here is a function call
    if (tmp->type == FUNCTION) {
        if (!checkParam(tmp->funcType.numParameters, tmp->funcType.parameters, FIRSTCHILD(expr), local, global))
            error(expr, MISSING_ARGUMENT, SEMANTIC_ERR);
        expr->type = tmp->funcType.returnType;
        return;
    }

    expr->type = tmp->type;
}

int checkParam(int numArgs, ParamType params[], Node *arguments, HashTable local, HashTable global) {
    if (arguments == NULL) return 1;

    Symbol *tmp;
    Node *argument = FIRSTCHILD(arguments);

    if (numArgs == 0)
        return 1;

    // no argument is passed.
    if (numArgs > 0 && argument == NULL) 
        return 0;

    for (int i = 0; i < numArgs; i++) {
        if (argument == NULL)
            return 0;
        
        if (argument->type == VOID_T)
            return 0;

        if (argument->nextSibling != NULL && i == numArgs - 1)
            return 0;

        switch (params[i].type) {
            case INT:
                if (argument->type != INT && argument->type != CHAR)
                    error(argument, WRONG_TYPE_ARGUMENT, SEMANTIC_ERR);
                break;
            case CHAR:
                if (argument->type == INT)
                    warning(argument, ASSIGN_INT_TO_CHAR);
                else if (argument->type != CHAR)
                    error(argument, WRONG_TYPE_ARGUMENT, SEMANTIC_ERR);
                break;
            case ARRAY:
                if (argument->label == Ident) {
                    tmp = lookupAll(local, global, argument->ident);
                    if (tmp == NULL)
                        error(argument, UNDEFINED_TOKEN, SEMANTIC_ERR); 
                    if (tmp->type != ARRAY)
                        error(argument, WRONG_TYPE_ARGUMENT, SEMANTIC_ERR);
                    if (tmp->arrType.baseType != params[i].arrType.baseType)
                        error(argument, WRONG_TYPE_ARGUMENT, SEMANTIC_ERR);
                }
                break;
            default:
                break;
        }

        argument = argument->nextSibling;
    }
    return 1;
}

void exprBinOp(Node *expr, HashTable local, HashTable global) {
    if (expr == NULL) return;

    if (FIRSTCHILD(expr) == NULL) return;

    switch (FIRSTCHILD(expr)->type) {
        case INT: case CHAR:
            if (SECONDCHILD(expr) == NULL) { // negative numbers
                expr->type = INT; 
                break;
            }
            if (SECONDCHILD(expr)->type == INT || SECONDCHILD(expr)->type == CHAR) {
                expr->type = INT;
            }
            else
                error(SECONDCHILD(expr), UNDEFINED_ERROR, SEMANTIC_ERR);
            break;
        case ARRAY:
            error(FIRSTCHILD(expr), UNDEFINED_ERROR, SEMANTIC_ERR);
            break;
        default:
            break;
    } 
}

void exprArray(Node *expr, HashTable local, HashTable global) {
    if (expr == NULL) return;

    char *ident = FIRSTCHILD(expr)->ident;
    Symbol *tmp = lookupAll(local, global, ident);
    if (tmp == NULL)
        error(FIRSTCHILD(expr), UNDEFINED_TOKEN, SEMANTIC_ERR);

    if (tmp->type != ARRAY)
        error(FIRSTCHILD(expr), NOT_ARRAY, SEMANTIC_ERR);

    Node *index = SECONDCHILD(expr);
    if (index->type != INT && index->type != CHAR)
        error(FIRSTCHILD(expr), WRONG_TYPE_INDEX, SEMANTIC_ERR);
        
    expr->type = tmp->arrType.baseType;
}

void exprCompOp(Node *expr, HashTable local, HashTable global) {
    if (expr == NULL)
        return;
    
    if (FIRSTCHILD(expr)->type == VOID_T)
        error(FIRSTCHILD(expr), INVALID_TYPE_TOKEN, SEMANTIC_ERR);
    
    if (SECONDCHILD(expr)->type == VOID_T)
        error(SECONDCHILD(expr), INVALID_TYPE_TOKEN, SEMANTIC_ERR);
    
    expr->type = INT; //? not sure
}

void exprNot(Node *expr, HashTable local, HashTable global) {
    if (expr == NULL)
        return;

    if (FIRSTCHILD(expr) == NULL) 
        return;
    
    if (FIRSTCHILD(expr)->type == VOID_T)
        error(FIRSTCHILD(expr), INVALID_TYPE_TOKEN, SEMANTIC_ERR);
    
    expr->type = INT; //? not sure
}