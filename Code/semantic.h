#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "symbol_table.h"
#include "node.h"


void Program_init(Node* root);
void ExtDefList(Node* root);
void ExtDef(Node* root);
void CompSt(Node* root,Type ret_type);//由于函数形参也是函数的局部作用域，因此Compst中将不压栈，请调用时自己看情况是否压栈！
void StmtList(Node* root,Type ret_type);
void Stmt(Node* root,Type ret_type);
void ExtDecList(Node* root,Type type);
void FunDec(Node* root,Type type);
FieldList ParamDec(Node*root);
Type Specifier(Node* root);//若类型定义中出问题，返回值为NULL，请注意这种情况！
void DefList(Node* root);
void Def(Node* root);
void DecList(Node*root,Type type);
void Dec(Node*root,Type type);
FieldList VarDec(Node*root,Type type);
Type Exp(Node*root);//返回该表达式值的最终类型
#endif