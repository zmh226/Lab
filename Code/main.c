#include "syntax.tab.h"
#include"node.h"
#include "symbol_table.h"
#include "semantic.h"
#include<stdio.h>
#include<stdlib.h>
int syntaxflag;
int flexflag;

 Node* createnode(int line1,int type,char* name1,char* ID1,int num1,double num2);
 
 /*增加该节点的孩子节点 */
 void CTchild(Node*node,Node* child);

/*增加该节点的兄弟节点 */
void CTsilbing(Node* node,Node* silbing);

void printtree(Node*root,int depth);
 Node* root;
 
int main(int argc, char** argv) 
{ 
  if (argc <= 1) return 1; 
  FILE* f = fopen(argv[1], "r"); 
  if (!f) 
  { 
  perror(argv[1]); 
  return 1; 
  } 
  syntaxflag=0;
  flexflag=0;
  root = NULL;
  yyrestart(f); 
  yyparse(); 
  if(flexflag==0&&syntaxflag==0)
  {
    //printtree(root,0);
    Program_init(root);
  }
  return 0; 
  }
