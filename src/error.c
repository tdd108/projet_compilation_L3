#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

void error(Node *errNode, ErrorType errType, ErrorCode errCode) {
    printf(ANSI_COLOR_RED "error " ANSI_COLOR_RESET);
    if (errNode != NULL) {
        switch (errType) {
            case UNDEFINED_TOKEN:
                printf("at line %d : undefined token '%s'", errNode->lineno, errNode->ident);
                break;
            case EXISTED_TOKEN:
                printf("at line %d : token '%s' already existed", errNode->lineno, errNode->ident);
                break;
            case MISSING_ARGUMENT:
                printf("at line %d : too few argument for function '%s'", errNode->lineno, errNode->ident);
                break;
            case WRONG_TYPE_ARGUMENT:
                printf("at line %d : wrong type of argument '%s'", errNode->lineno, errNode->ident);
                break;
            case WRONG_TYPE_FUNCTION:
                printf("at line %d : '%s'", errNode->lineno, errNode->ident);
                break;
            case WRONG_TYPE_INDEX:
                printf("at line %d : token '%s' is not right type of index", errNode->lineno, errNode->ident);
                break;
            case WRONG_TYPE_RETURN:
                printf("at line %d : wrong type return '%s'", errNode->lineno, errNode->ident);
                break;
            case INVALID_TYPE_TOKEN:
                printf("at line %d : invalid type token '%s'", errNode->lineno, errNode->ident);
                break;
            case NOT_ARRAY:
                printf("at line %d : token '%s' is not array type", errNode->lineno, errNode->ident);
                break;
            case NOT_FUNCTION:
                printf("at line %d : token '%s' is not function type", errNode->lineno, errNode->ident);
                break;
            case ASSIGN_TO_VOID_FUNCTION:
                printf("at line %d : assign void function to token '%s'", errNode->lineno, errNode->ident);
                break;
            case UNMODIFIABLE_LVALUE:
                printf("at line %d : '%s' unmodifiable", errNode->lineno, errNode->ident);
                break;
            case MISSING_MAIN:
                printf("missing function main");
                break;
            default:
                printf("at line %d : '%s'", errNode->lineno, errNode->ident);
                break;
        }
    }
    printf("\n");
    exit(errCode);
}

void warning(Node *warningNode, WarningType warning) {
    printf(ANSI_COLOR_YELLOW "warning: " ANSI_COLOR_RESET);
    if (warningNode != NULL) {
        switch (warning) {
        case ASSIGN_INT_TO_CHAR:
            printf("at line %d : assign int to char type token '%s'", warningNode->lineno, warningNode->ident);
            break;
        default:
            break;
        }
    }
    printf("\n");
}