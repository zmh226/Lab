%{
    int line=1;
%}
digit [0-9]
letter [a-z_A-Z]
INT [+-]?(0|[1-9]{digit}*)
FLOAT [+-]?([0-9]*\.[0-9]*)|[+-]?([0-9]*\.?[0-9]+|[0-9]+\.)[eE][+-]?{digit}+
ID {letter}[0-9a-z_A-Z]*
SEMI ";"
COMMA ","
ASSIGNOP "="
RELOP >|<|>=|<=|==|!=
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
{SEMI} {printf("SEMI\n");}
{COMMA} {printf("COMMA\n");}
{ASSIGNOP} {printf("ASSIGNOP\n");}
{RELOP} {printf("RELOP\n");}
{PLUS} {printf("PLUS\n");}
{MINUS} {printf("MINUS\n");}
{STAR} {printf("STAR\n");}
{DIV} {printf("DIV\n");}
{AND} {printf("AND\n");}
{OR} {printf("OR\n");}
{DOT} {printf("DOT\n");}
{NOT} {printf("NOT\n");}
{TYPE} {printf("%s\n",yytext);}
{LB} {printf("LB\n");}
{RB} {printf("RB\n");}
{LP} {printf("LP\n");}
{RP} {printf("RP\n");}
{LC} {printf("LC\n");}
{RC} {printf("RC\n");}
{STRUCT} {printf("STRUCT\n");}
{RETURN} {printf("RETURN\n");}
{IF} {printf("IF\n");}
{ELSE} {printf("ELSE\n");}
{WHILE} {printf("WHILE\n");}
{ID} {printf("%s\n",yytext);}
{INT} {printf("int num:%d\n",atoi(yytext));}
{FLOAT} {printf("float num:%f\n",atof(yytext));}
{block} {;}
{digit}{letter}+ {printf("Error type A at line %d: Illegal format of id \'%s\'\n",line,yytext);}
[+-]?([0-9]*\.?[0-9]+|[0-9]+\.)[eE][+-]?|[+-]?([0-9]*\.?[0-9]+|[0-9]+\.)[eE][+-]?{letter}[a-zA-z0-9]* {printf("Error type A at line %d:  Illegal floating point number \'%s\'\n",line,yytext);}
{FLOAT}{FLOAT}+ {printf("Error type A at line %d: Illegal floating point number \'%s\'\n",line,yytext);}
. {printf("Error type A at line %d: Mysterious characters \'%s\'\n",line,yytext);}

%%

int main(int argc, char** argv) {
if (argc > 1) {
if (!(yyin = fopen(argv[1], "r"))) {
perror(argv[1]);
return 1;
}
}
while (yylex() != 0);
printf("%d\n",line);
return 0;
}