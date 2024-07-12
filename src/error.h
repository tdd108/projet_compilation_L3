#ifndef ERROR_H
#define ERROR_H

#include "tree.h"

#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef enum {
    SEMANTIC_ERR = 2,
    UNDEFINED_ERROR_CODE = 3,
} ErrorCode;

typedef enum {
    EXISTED_TOKEN,
    UNDEFINED_TOKEN,
    WRONG_TYPE_ARGUMENT,
    WRONG_TYPE_FUNCTION,
    WRONG_TYPE_INDEX,
    WRONG_TYPE_RETURN,
    INVALID_TYPE_TOKEN,
    NOT_ARRAY,
    NOT_FUNCTION,
    ASSIGN_TO_VOID_FUNCTION,
    UNMODIFIABLE_LVALUE,
    MISSING_ARGUMENT,
    MISSING_MAIN,
    UNDEFINED_ERROR,
} ErrorType;

typedef enum {
    ASSIGN_INT_TO_CHAR,
} WarningType;

void error(Node *errNode, ErrorType errType, ErrorCode errCode);

void warning(Node *warningNode, WarningType warning);

#endif