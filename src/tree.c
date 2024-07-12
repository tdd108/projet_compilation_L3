/* tree.c */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
extern int lineno;       /* from lexer */

static const char *StringFromLabel[] = {
  FOREACH_LABEL(GENERATE_STRING)
  /* list all other node labels, if any */
  /* The list must coincide with the label_t enum in tree.h */
  /* To avoid listing them twice, see https://stackoverflow.com/a/10966395 */
};

Node *makeNode(label_t label, char *value) {
  Node *node = malloc(sizeof(Node));
  if (!node) {
    printf("Run out of memory\n");
    exit(1);
  }
  node->label = label;
  if (value != NULL) {
    switch (label) {
    case num:
      node->intValue = atoi(value);
      break;
    case Character:
      strcpy(node->charValue, value);
      break;
    case Ident:
      strcpy(node->ident, value);
      break;
    case Type:
      strcpy(node->name, value);
      break;
    case DivStar: case AddSub:
      node->bin_op = value[0];
      break;
    case Eq: case Order:
      strcpy(node->comp_op, value);
      break;
    default:
      break;
    }
  }
  node-> firstChild = node->nextSibling = NULL;
  node->lineno=lineno;
  return node;
}

void addSibling(Node *node, Node *sibling) {
  Node *curr = node;
  while (curr->nextSibling != NULL) {
    curr = curr->nextSibling;
  }
  curr->nextSibling = sibling;
}

void addChild(Node *parent, Node *child) {
  if (parent->firstChild == NULL) {
    parent->firstChild = child;
  }
  else {
    addSibling(parent->firstChild, child);
  }
}

void deleteTree(Node *node) {
  if (node->firstChild) {
    deleteTree(node->firstChild);
  }
  if (node->nextSibling) {
    deleteTree(node->nextSibling);
  }
  free(node);
}

void printTree(Node *node) {
  static bool rightmost[128]; // tells if node is rightmost sibling
  static int depth = 0;       // depth of current node
  for (int i = 1; i < depth; i++) { // 2502 = vertical line
    printf(rightmost[i] ? "    " : "\u2502   ");
  }
  if (depth > 0) { // 2514 = L form; 2500 = horizontal line; 251c = vertical line and right horiz 
    printf(rightmost[depth] ? "\u2514\u2500\u2500 " : "\u251c\u2500\u2500 ");
  }
  printf("%s", StringFromLabel[node->label]);
  switch (node->label) {
  case num:
    printf(": %d", node->intValue);
    break;
  case Character:
    printf(": %s", node->charValue);
    break;
  case Ident:
    printf(": %s", node->ident);
    break;
  case Type:
    printf(": %s", node->name);
    break;
  case DivStar: case AddSub:
    printf(": %c", node->bin_op);
    break;
  case Eq: case Order:
    printf(": %s", node->comp_op);
    break;
  default:
    break;
  }
  printf("\n");
  depth++;
  for (Node *child = node->firstChild; child != NULL; child = child->nextSibling) {
    rightmost[depth] = (child->nextSibling) ? false : true;
    printTree(child);
  }
  depth--;
}
