#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "symbol_table.h"
#include "node.h"


void Program_init(Node* root);
void ExtDefList(Node* root);
void ExtDef(Node* root);
void Specifier(Node* root);
void Compst(Node* root);
void ExtDecList(Node* root);
void FunDec(Node* root);
#endif