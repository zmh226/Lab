%{ 
 #include "lex.yy.c" 
 
 extern int syntaxflag;
 extern int flexflag;
 void yyerror(char* mag);
 
 Node* createnode(int line1,int type,char* name1,char* ID1,int num1,double num2);
 
 /*增加该节点的孩子节点 */
 void CTchild(Node*node,Node* child);

/*增加该节点的兄弟节点 */
void CTsilbing(Node* node,Node* silbing);

void printtree(Node*root,int depth);
%} 
%union{
   int type_int;
   struct TreeNode* node;
}
%token<node> INT FLOAT
%token<node> ID
%token<node> SEMI COMMA
%token<node> ASSIGNOP PLUS MINUS DIV STAR AND OR
%token<node> DOT NOT
%token<node> RELOP
%token<node> TYPE STRUCT RETURN IF ELSE WHILE
%token<node> LP RP LB RB LC RC

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left DIV STAR
%right NOT
%left LP RP LB RB DOT

%type<node> Program ExtDefList ExtDef ExtDecList Specifier FunDec CompSt VarDec
%type<node> StructSpecifier OptTag DefList Tag VarList ParamDec StmtList Stmt Exp Dec Def DecList Args
%%
Program:ExtDefList {$$=createnode($1->sline,0,"Program","",0,0);
CTchild($$,$1);
root=$$;
};
ExtDefList:ExtDef ExtDefList{$$=createnode($1->sline,0,"ExtDefList","",0,0);CTchild($$,$1);CTsilbing($1,$2);}
|/* empty */{$$=NULL;}; 
ExtDef : Specifier ExtDecList SEMI {$$=createnode($1->sline,0,"ExtDef","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);}
| Specifier SEMI {$$=createnode($1->sline,0,"ExtDef","",0,0);CTchild($$,$1);CTsilbing($1,$2);}
| Specifier FunDec CompSt {$$=createnode($1->sline,0,"ExtDef","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);};
ExtDecList : VarDec {$$=createnode($1->sline,0,"ExtDecList","",0,0);CTchild($$,$1);}
| VarDec COMMA ExtDecList{$$=createnode($1->sline,0,"ExtDecList","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);};
Specifier : TYPE {$$=createnode($1->sline,0,"Specifier","",0,0);CTchild($$,$1);}
| StructSpecifier {$$=createnode($1->sline,0,"Specifier","",0,0);CTchild($$,$1);};
StructSpecifier : STRUCT OptTag LC DefList RC  {$$=createnode($1->sline,0,"StructSpecifier","",0,0);CTchild($$,$1);
                                               if($2==NULL) CTsilbing($1,$3);
                                               CTsilbing($1,$2);
                                               CTsilbing($2,$3);
                                               if($4==NULL) CTsilbing($3,$5);
                                               CTsilbing($3,$4);
                                               CTsilbing($4,$5);
                                               }
| STRUCT Tag  {$$=createnode($1->sline,0,"StructSpecifier","",0,0);CTchild($$,$1);CTsilbing($1,$2);};
OptTag : ID {$$=createnode($1->sline,0,"OptTag","",0,0);CTchild($$,$1);}
| /* empty */{$$=NULL;};
Tag : ID{$$=createnode($1->sline,0,"Tag","",0,0);CTchild($$,$1);};
VarDec : ID {$$=createnode($1->sline,0,"VarDec","",0,0);CTchild($$,$1);}
| VarDec LB INT RB {$$=createnode($1->sline,0,"VarDec","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);CTsilbing($3,$4);};
FunDec : ID LP VarList RP {$$=createnode($1->sline,0,"FunDec","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);CTsilbing($3,$4);};
| ID LP RP {$$=createnode($1->sline,0,"FunDec","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);};
VarList : ParamDec COMMA VarList  {$$=createnode($1->sline,0,"VarList","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);}
| ParamDec {$$=createnode($1->sline,0,"VarList","",0,0);CTchild($$,$1);};
ParamDec : Specifier VarDec {$$=createnode($1->sline,0,"ParamDec","",0,0);CTchild($$,$1);CTsilbing($1,$2);};
CompSt : LC DefList StmtList RC {$$=createnode($1->sline,0,"CompSt","",0,0);CTchild($$,$1);
                                 if($2!=NULL&&$3!=NULL)
                                     {CTsilbing($1,$2);CTsilbing($2,$3);CTsilbing($3,$4);}
                                 else if($3!=NULL &&$2==NULL)
                                     {CTsilbing($1,$3);CTsilbing($3,$4);}
                                 else if($2!=NULL &&$3==NULL)
                                     {CTsilbing($1,$2);CTsilbing($2,$4);}
                                 else
                                     {CTsilbing($1,$4);}};
StmtList : Stmt StmtList  {$$=createnode($1->sline,0,"StmtList","",0,0);CTchild($$,$1);CTsilbing($1,$2);}
| /* empty */{$$=NULL;};
Stmt : Exp SEMI {$$=createnode($1->sline,0,"Stmt","",0,0);CTchild($$,$1);CTsilbing($1,$2);}
| CompSt {$$=createnode($1->sline,0,"Stmt","",0,0);CTchild($$,$1);}
| RETURN Exp SEMI {$$=createnode($1->sline,0,"Stmt","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);}
| IF LP Exp RP Stmt {$$=createnode($1->sline,0,"Stmt","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);CTsilbing($3,$4);CTsilbing($4,$5);}
| IF LP Exp RP Stmt ELSE Stmt {$$=createnode($1->sline,0,"Stmt","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);CTsilbing($3,$4);CTsilbing($4,$5);CTsilbing($5,$6);CTsilbing($6,$7);}
| WHILE LP Exp RP Stmt {$$=createnode($1->sline,0,"Stmt","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);CTsilbing($3,$4);CTsilbing($4,$5);};
DefList : Def DefList {$$=createnode($1->sline,0,"DefList","",0,0);CTchild($$,$1);CTsilbing($1,$2);}
| /* empty */{$$=NULL;}; 
Def : Specifier DecList SEMI {$$=createnode($1->sline,0,"Def","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);};
DecList : Dec {$$=createnode($1->sline,0,"DecList","",0,0);CTchild($$,$1);}
| Dec COMMA DecList {$$=createnode($1->sline,0,"DecList","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);};
Dec : VarDec {$$=createnode($1->sline,0,"Dec","",0,0);CTchild($$,$1);}
| VarDec ASSIGNOP Exp{$$=createnode($1->sline,0,"Dec","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);};
Exp : Exp ASSIGNOP Exp {$$=createnode($1->sline,0,"Exp","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);}
| Exp AND Exp {$$=createnode($1->sline,0,"Exp","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);}
| Exp OR Exp {$$=createnode($1->sline,0,"Exp","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);}
| Exp RELOP Exp {$$=createnode($1->sline,0,"Exp","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);}
| Exp PLUS Exp {$$=createnode($1->sline,0,"Exp","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);}
| Exp MINUS Exp {$$=createnode($1->sline,0,"Exp","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);}
| Exp STAR Exp {$$=createnode($1->sline,0,"Exp","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);}
| Exp DIV Exp {$$=createnode($1->sline,0,"Exp","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);}
| LP Exp RP {$$=createnode($1->sline,0,"Exp","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);}
| MINUS Exp {$$=createnode($1->sline,0,"Exp","",0,0);CTchild($$,$1);CTsilbing($1,$2);}
| NOT Exp {$$=createnode($1->sline,0,"Exp","",0,0);CTchild($$,$1);CTsilbing($1,$2);}
| ID LP Args RP {$$=createnode($1->sline,0,"Exp","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);CTsilbing($3,$4);}
| ID LP RP {$$=createnode($1->sline,0,"Exp","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);}
| Exp LB Exp RB {$$=createnode($1->sline,0,"Exp","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);CTsilbing($3,$4);}
| Exp DOT ID {$$=createnode($1->sline,0,"Exp","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);}
| ID {$$=createnode($1->sline,0,"Exp","",0,0);CTchild($$,$1);}
| INT {$$=createnode($1->sline,0,"Exp","",0,0);CTchild($$,$1);}
| FLOAT {$$=createnode($1->sline,0,"Exp","",0,0);CTchild($$,$1);};
Args : Exp COMMA Args {$$=createnode($1->sline,0,"Args","",0,0);CTchild($$,$1);CTsilbing($1,$2);CTsilbing($2,$3);}
| Exp {$$=createnode($1->sline,0,"Args","",0,0);CTchild($$,$1);};

%%

 
 Node* createnode(int line1,int type,char* name1,char* ID1,int num1,double num2){
     Node* newnode = (Node*)malloc(sizeof(Node));
     newnode-> sline = line1;
     newnode-> stype = type;
     newnode -> name = (char*)malloc((strlen(name1)+1)*sizeof(char));
     strcpy(newnode->name,name1);
     newnode -> sID =  (char*)malloc((strlen(ID1)+1)*sizeof(char));
     strcpy(newnode->sID,ID1);
     newnode-> snum = num1;
     newnode-> snum1 = num2;
     newnode -> child = NULL;
     newnode -> silbing = NULL;
     return newnode;
 }
 
 /*增加该节点的孩子节点 */
 void CTchild(Node*node,Node* child){
      if(node !=NULL && child!=NULL){
          node->child = child;
          node->sline = child->sline;
      }
}

/*增加该节点的兄弟节点 */
void CTsilbing(Node* node,Node* silbing){
       if(node !=NULL && silbing!=NULL){
          node->silbing = silbing;
      }
}
void printtree(Node*root,int depth){
    if(root == NULL) return;
    for(int i =0;i<depth;i++){
       printf("  ");
    }
    if(root->stype==0){
       printf("%s (%d)\n",root->name,root->sline);
}
   if(root->stype==1){
   printf("%s\n",root->name);
   }
   if(root->stype ==2 || root->stype==3){
       printf("%s: %s\n",root->name,root->sID);
   }
    if(root->stype==4){
       printf("%s: %d\n",root->name,root->snum);
    }
    if(root->stype==5){
    printf("%s: %f\n",root->name,root->snum1);
    }
    printtree(root->child,depth+1);
    printtree(root->silbing,depth);
}
void yyerror(char* mag){
  if(flexflag==0){
  syntaxflag=1;
  printf("Error type B at line %d:%s.\n",line,mag);}
}

