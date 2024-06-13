#ifndef INTER_CODE
#define INTER_CODE

#include"semantic.h"
#include"object_code.h"
extern int num_temp ;
extern int num_label ;
extern int num_variable;


char*newtemp();
char*newlabel();
char*newvariable();
int getSize(Type type);//获取该类型的大小
void trans_Program_init(Node* root,FILE*file);
void trans_ExtDefList(Node* root,FILE*file);
void trans_ExtDef(Node* root,FILE*file);
void trans_FunDec(Node* root,FILE*file);
void trans_CompSt(Node* root,FILE*file);
void trans_DefList(Node* root,FILE*file);
void trans_Def(Node* root,FILE*file);
void trans_DecList(Node* root,FILE* file);
void trans_Dec(Node* root,FILE* file);
void trans_StmtList(Node*root,FILE*file);
void trans_Stmt(Node*root,FILE*file);
void trans_Cond(Node*root,char*label_true,char*label_false,FILE* file);
void trans_Exp(Node* root,FILE* file,char* place);
void trans_Cond(Node*root,char*label_true,char*label_false,FILE* file);
void transaddr_Exp(Node* root,FILE*file,char* place);
void trans_Args(Node* root,FILE* file,FieldList* arg);
FieldList serchvarname(char* name);//查找对应的变量的FieldList
#endif