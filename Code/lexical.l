%{
    #include<stdio.h>
    #include<stdlib.h>
    #include<string.h>
    #include "syntax.tab.h"
    #include"node.h"
    extern int flexflag;
    int line=1;
    extern Node* root;
    
 Node* createnode(int line1,int type,char* name1,char* ID1,int num1,double num2);
 
 /*增加该节点的孩子节点 */
 void CTchild(Node*node,Node* child);

/*增加该节点的兄弟节点 */
void CTsilbing(Node* node,Node* silbing);

void printtree(Node*root,int depth);
%}
digit [0-9]
letter [a-z_A-Z]
INT 0|([1-9]{digit}*)
FLOAT ([0-9]+\.[0-9]+)|((([0-9]*\.[0-9]+)|([0-9]+\.))[eE][+-]?{digit}+)
ID {letter}[0-9a-z_A-Z]*
SEMI ";"
COMMA ","
ASSIGNOP "="
RELOP ">"|"<"|">="|"<="|"=="|"!="
PLUS "+"
MINUS "-"
STAR "*"
DIV "/"
AND "&"
OR "||"
DOT "."
NOT "!"
TYPE int|float
LP "("
RP ")"
LB "["
RB "]"
LC "{"
RC "}"
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
enter "\n"
block [ \t\r]
%%
{enter} {line++;}
{SEMI} {yylval.node=createnode(line,1,"SEMI","",0,0);return SEMI;}
{COMMA} {yylval.node=createnode(line,1,"COMMA","",0,0);return COMMA;}
{ASSIGNOP} {yylval.node=createnode(line,1,"ASSIGNOP","",0,0);return ASSIGNOP;}
{RELOP} {yylval.node=createnode(line,1,"RELOP","",0,0);return RELOP;}
{PLUS} {yylval.node=createnode(line,1,"PLUS","",0,0);return PLUS;}
{MINUS} {yylval.node=createnode(line,1,"MINUS","",0,0);return MINUS;}
{STAR} {yylval.node=createnode(line,1,"STAR","",0,0);return STAR;}
{DIV} {yylval.node=createnode(line,1,"DIV","",0,0);return DIV;}
{AND} {yylval.node=createnode(line,1,"AND","",0,0);return AND;}
{OR} {yylval.node=createnode(line,1,"OR","",0,0);return OR;}
{DOT} {yylval.node=createnode(line,1,"DOT","",0,0);return DOT;}
{NOT} {yylval.node=createnode(line,1,"NOT","",0,0);return NOT;}
{TYPE} {yylval.node=createnode(line,3,"TYPE",yytext,0,0);return TYPE;}
{LB} {yylval.node=createnode(line,1,"LB","",0,0);return LB;}
{RB} {yylval.node=createnode(line,1,"RB","",0,0);return RB;}
{LP} {yylval.node=createnode(line,1,"LP","",0,0);return LP;}
{RP} {yylval.node=createnode(line,1,"RP","",0,0);return RP;}
{LC} {yylval.node=createnode(line,1,"LC","",0,0);return LC;}
{RC} {yylval.node=createnode(line,1,"RC","",0,0);return RC;}
{STRUCT} {yylval.node=createnode(line,1,"STRUCT","",0,0);return STRUCT;}
{RETURN} {yylval.node=createnode(line,1,"RETURN","",0,0);return RETURN;}
{IF} {yylval.node=createnode(line,1,"IF","",0,0);return IF;}
{ELSE} {yylval.node=createnode(line,1,"ELSE","",0,0);return ELSE;}
{WHILE} {yylval.node=createnode(line,1,"WHILE","",0,0);return WHILE;}
{ID} {yylval.node=createnode(line,2,"ID",yytext,0,0);return ID;}
{INT} {yylval.node=createnode(line,4,"INT","",atoi(yytext),0);return INT;}
{FLOAT} {yylval.node=createnode(line,5,"FLOAT","",0,atof(yytext));return FLOAT;}
{block} {;}
((([0-9]*\.[0-9]+)|([0-9]+\.))[eE][+-]?)|((([0-9]*\.[0-9]+)|([0-9]+\.))[eE][+-]?{letter}[a-zA-z0-9]*) {printf("Error type A at line %d:  Illegal floating point number \'%s\'\n",line,yytext);flexflag=1;}
. {printf("Error type A at line %d: Mysterious characters \'%s\'\n",line,yytext);flexflag=1;}

%%


