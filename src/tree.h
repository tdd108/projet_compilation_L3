/* tree.h */
#ifndef TREE_H
#define TREE_H

#define FOREACH_LABEL(LABEL) \
        LABEL(Else) \
        LABEL(While) \
        LABEL(instr) \
        LABEL(Instructions) \
        LABEL(Instruction) \
        LABEL(Parameters) \
        LABEL(ListExp) \
        LABEL(And) \
        LABEL(Program) \
        LABEL(Body) \
        LABEL(num) \
        LABEL(DivStar) \
        LABEL(Character) \
        LABEL(Void) \
        LABEL(Order) \
        LABEL(Or) \
        LABEL(AddSub) \
        LABEL(If) \
        LABEL(Declaration) \
        LABEL(Declarations) \
        LABEL(Functions) \
        LABEL(Function) \
        LABEL(Eq) \
        LABEL(Header) \
        LABEL(Type) \
        LABEL(Return) \
        LABEL(not) \
        LABEL(Ident) \
        LABEL(Arguments) \
        LABEL(Assign) \
        LABEL(Not) \
        LABEL(Array) \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

typedef enum {
  FOREACH_LABEL(GENERATE_ENUM)
} label_t;

typedef enum {
    VOID_T,
    INT,
    CHAR,
    ARRAY,
    FUNCTION
} type_t;

typedef struct Node {
  label_t label;
  type_t type;
  char true_t[10];
  char false_t[10];
  union {
    int intValue;
    char charValue[5];
    char bin_op;
    char comp_op[2];
    char ident[64];
    char name[64];
  };
  int lineno;
  struct Node *firstChild, *nextSibling;
} Node;

Node *makeNode(label_t label, char *value);
void addSibling(Node *node, Node *sibling);
void addChild(Node *parent, Node *child);
void deleteTree(Node*node);
void printTree(Node *node);

#define FIRSTCHILD(node) node->firstChild
#define SECONDCHILD(node) node->firstChild->nextSibling
#define THIRDCHILD(node) node->firstChild->nextSibling->nextSibling

#endif