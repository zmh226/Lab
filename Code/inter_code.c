#include "inter_code.h"

int num_temp = 1;
int num_label = 1;
int num_variable = 1;
int getSize(Type type){
    if(type->kind==BASIC){
        return 4;
    }
    else if(type->kind==ARRAY){
        return type->u.array.size*getSize(type->u.array.elem);
    }
    else if(type->kind ==STRUCTURE){
        int size0=0;
        FieldList temp = type->u.structures.structure;
        while(temp!=NULL){
            size0+=getSize(temp->type);
            temp=temp->tail;
        }
        return size0;
    }
    return 0;
}

char* newtemp(){
    char str[10];
    sprintf(str,"%d",num_temp);
    char* temp1= (char*)malloc((strlen(str)+2)*sizeof(char));
    strcpy(temp1,"t");
    strcat(temp1,str);
    num_temp++;
    return temp1;
}

char* newlabel(){
    char str[10];
    sprintf(str,"%d",num_label);
    char* temp1= (char*)malloc((strlen(str)+6)*sizeof(char));
    strcpy(temp1,"label");
    strcat(temp1,str);
    num_label++;
    return temp1;
}

char* newvariable(){
    char str[10];
    sprintf(str,"%d",num_variable);
    char* temp1= (char*)malloc((strlen(str)+2)*sizeof(char));
    strcpy(temp1,"v");
    strcat(temp1,str);
    num_variable++;
    return temp1;
}

FieldList serchvarname(char* name){
    unsigned val = hash_pjw(name);
        if(hash_table[val] == NULL) return NULL;
        else{
            HashNode temp=hash_table[val];
            while(temp!=NULL){
                if((strcmp(temp->data->name,name)==0) && temp->data->type->kind != DEFINITION && temp->data->type->kind != FUNCTION){
                    //printf("%s\n",temp->data->name);
                    return temp->data;
                }
                temp=temp->next_in_bucket;
            }
        }
        return NULL;
    }
void trans_Program_init(Node* root,FILE*file){
    //Program → ExtDefList
    trans_ExtDefList(root->child,file);
}

void trans_ExtDefList(Node* root,FILE*file){
    //ExtDefList → ExtDef ExtDefList | empty
    if(root==NULL) return;
    //printf("%d\n",root->sline);
    if(root->child != NULL){
       trans_ExtDef(root->child,file);
       trans_ExtDefList(root->child->silbing,file);
    }
}

void trans_ExtDef(Node* root,FILE*file){
 //   ExtDef → Specifier ExtDecList SEMI 
//| Specifier SEMI 
//| Specifier FunDec CompSt
   if(root==NULL)
   return;
   Node* t1=root->child;
   Node* t2=NULL;
   Node* t3=NULL;
   if(t1!=NULL){
        t2 = t1->silbing;
    }
    if(t2!=NULL){
        t3 = t2->silbing;
    }
    if(t2!=NULL &&strcmp(t2->name,"FunDec")==0)
    {
       trans_FunDec(t2,file);
       trans_CompSt(t3,file);
       fprintf(file,"\n");
    }
}

void trans_FunDec(Node* root,FILE*file){
    //FunDec → ID LP VarList RP 
//| ID LP RP 
//return;
   Node*n1=root->child;
   fprintf(file,"FUNCTION %s :\n",n1->sID);
   Type type1=serchfunc(n1->sID);
   int num1=type1->u.function.num_of_parameter;
   FieldList param1=type1->u.function.parameters;
   while(num1>0){
    param1->inter=newvariable();
    fprintf(file,"PARAM %s\n",param1->inter);
    param1=param1->tail;
    num1--;
   }
}

void trans_CompSt(Node* root,FILE*file){
    //CompSt → LC DefList StmtList RC
    Node* t1=root->child->silbing;
    Node* t2=NULL;
    Node* t3=NULL;
    if(t1->silbing!=NULL)
    {
        t2=t1->silbing;
        if(strcmp(t1->name,"DefList")==0) trans_DefList(t1,file);
        else if(strcmp(t1->name,"StmtList")==0) trans_StmtList(t1,file);
        if (t2->silbing!=NULL)
        {
            if(strcmp(t2->name,"StmtList")==0) trans_StmtList(t2,file);
        }
    }
}

void trans_DefList(Node* root,FILE*file){
    //DefList → Def DefList |empty
    Node*t1=root->child;
     if(t1==NULL) return;
     Node*t2=t1->silbing;
     if(t1!=NULL){
        trans_Def(t1,file);
        if(t2!=NULL)
           trans_DefList(t2,file);
     }
}

void trans_Def(Node* root,FILE*file){//Def → Specifier DecList SEMI
   Node* t1=root->child;
   Node* t2=t1->silbing;
   if(t1 == NULL) return;
   if(t2!=NULL)
      trans_DecList(t2,file);
}

void trans_DecList(Node* root,FILE* file){//DecList → Dec | Dec COMMA DecList
   Node*t1=root->child;
   if(t1!=NULL){
    trans_Dec(t1,file);
    if(t1->silbing != NULL)
       trans_DecList(t1->silbing->silbing,file);
   }
}

void trans_Dec(Node* root,FILE* file){//Dec → VarDec | VarDec ASSIGNOP Exp 
   Node*t1=root->child;
   if(t1->silbing!=NULL){
    if(strcmp(t1->child->name,"ID")==0){
        FieldList newlist1=serchvarname(t1->child->sID);
        if(newlist1->inter==NULL){
            newlist1->inter = newvariable();
        }
        trans_Exp(t1->silbing->silbing,file,newlist1->inter);
    }
    Node*n1=t1->child;
    while(strcmp(n1->name,"ID")!=0){
        n1=n1->child;
    }
    FieldList newlist1=serchvarname(n1->sID);
    if(newlist1->inter==NULL){
        newlist1->inter=newvariable();
    }
    if(newlist1->type->kind == STRUCTURE || newlist1->type->kind == ARRAY){
        int size1= getSize(newlist1->type);
        fprintf(file,"DEC %s %d\n",newlist1->inter,size1);
    }
   }
   else
   {
    Node*n1=t1->child;
    while(strcmp(n1->name,"ID")!=0){
        n1=n1->child;
    }
    FieldList newlist1=serchvarname(n1->sID);
    if(newlist1->inter==NULL){
        newlist1->inter=newvariable();
    }
    if(newlist1->type->kind == STRUCTURE || newlist1->type->kind == ARRAY){
        int size1= getSize(newlist1->type);
        fprintf(file,"DEC %s %d\n",newlist1->inter,size1);
    }
   }
}

void trans_StmtList(Node*root,FILE*file){//StmtList → Stmt StmtList | empty
//return;
    if(root==NULL) return;
    //printf("stmtlistline: %d\n",root->sline);
    Node*t1=root->child;   
    if(t1!=NULL)
    {
        //return;
        trans_Stmt(t1,file);
        Node*t2=t1->silbing;
        if(t2!=NULL) trans_StmtList(t2,file);
    }
}

void trans_Stmt(Node*root,FILE*file){
    Node*n1=root->child;
    //Stmt → Exp SEMI 
    if(strcmp(n1->name,"Exp")==0){
        trans_Exp(n1,file,NULL);
    }
//| CompSt 
    else if(strcmp(n1->name,"CompSt")==0){
        trans_CompSt(n1,file);
    }
//| RETURN Exp SEMI 
   else if(strcmp(n1->name,"RETURN")==0){
     Node*n2=n1->silbing;
     if(strcmp(n2->child->name,"INT")==0){
        fprintf(file,"RETURN #%d\n",n2->child->snum);
     }
     else if(strcmp(n2->child->name,"ID")==0 && n2->child->silbing==NULL){
        FieldList newlist1 = serchvarname(n2->child->sID);
        fprintf(file,"RETURN %s\n",newlist1->inter);
     }
     else{
     char*t1=newtemp();
     trans_Exp(n1->silbing,file,t1);
     fprintf(file,"RETURN %s\n",t1);
     }
   } 
 else if(strcmp(n1->name,"IF")==0){
    Node*n3=n1->silbing->silbing->silbing->silbing->silbing;//NULL/ELSE
    Node*n4=n1->silbing->silbing;//EXP
    Node*n5=n4->silbing->silbing;//STMT after RP
    if(n3==NULL){//| IF LP Exp RP Stmt
       char* label11=newlabel();
       char* label12=newlabel();
       trans_Cond(n4,label11,label12,file);
       fprintf(file,"LABEL %s :\n",label11);
       trans_Stmt(n5,file);
       fprintf(file,"LABEL %s :\n",label12);
    }
    else{//| IF LP Exp RP Stmt ELSE Stmt 
       char* label11=newlabel();
       char* label12=newlabel();
       char* label13=newlabel();
       trans_Cond(n4,label11,label12,file);
       fprintf(file,"LABEL %s :\n",label11);
       trans_Stmt(n5,file);
       fprintf(file,"GOTO %s\n",label13);
       fprintf(file,"LABEL %s :\n",label12);
       trans_Stmt(n3->silbing,file);
       fprintf(file,"LABEL %s :\n",label13);
    }
 }
 else if(strcmp(n1->name,"WHILE")==0){
//| WHILE LP Exp RP Stmt
    Node*n8=n1->silbing->silbing;
    Node*n9=n8->silbing->silbing;
    char* label11=newlabel();
    char* label12=newlabel();
    char* label13=newlabel();
    fprintf(file,"LABEL %s :\n",label11);
    trans_Cond(n8,label12,label13,file);
    fprintf(file,"LABEL %s :\n",label12);
    trans_Stmt(n9,file);
    fprintf(file,"GOTO %s\n",label11);
    fprintf(file,"LABEL %s :\n",label13);
 }
}

void trans_Cond(Node*root,char*label_true,char*label_false,FILE* file){ 
    Node*n1=root->child;
    Node*n2=NULL;
    Node*n3=NULL;
    if(n1->silbing!=NULL){
        n2=n1->silbing;
        if(n2->silbing!=NULL)
          n3=n2->silbing;
    }
    //Exp →Exp RELOP Exp
    if(n2!=NULL && strcmp(n2->name,"RELOP")==0){
        char*t1=newtemp();
        char*t2=newtemp();
        trans_Exp(n1,file,t1);
        trans_Exp(n3,file,t2);
        fprintf(file,"IF %s %s %s GOTO %s\n",t1,n2->sID,t2,label_true);
        fprintf(file,"GOTO %s\n",label_false);
    }
    //| NOT Exp
    else if(strcmp(n1->name,"NOT")==0){
        trans_Cond(n2,label_false,label_true,file);
    }
    //Exp AND Exp
    else if(n2!=NULL && strcmp(n2->name,"AND")==0){
        char*label11=newlabel();
        trans_Cond(n1,label11,label_false,file);
        fprintf(file,"LABEL %s :\n",label11);
        trans_Cond(n3,label_true,label_false,file);
    }
    //Exp OR Exp 
    else if(n2!=NULL && strcmp(n2->name,"OR")==0){
        char*label11=newlabel();
        trans_Cond(n1,label_true,label11,file);
        fprintf(file,"LABEL %s :\n",label11);
        trans_Cond(n3,label_true,label_false,file);
    }
    else{
        char*t1=newtemp();
        trans_Exp(root,file,t1);
        fprintf(file,"IF %s != #0 GOTO %s\n",t1,label_true);
        fprintf(file,"GOTO %s\n",label_false);
    }
}

void trans_Exp(Node* root,FILE* file,char* place){
    Node* n1= root->child;
    Node* n2=NULL;
    Node* n3 =NULL;
    Node* n4=NULL;
    if(n1->silbing!=NULL){
        n2=n1->silbing;
        if(n2->silbing!=NULL){
            n3=n2->silbing;
            if(n3->silbing!=NULL)
             n4=n3->silbing;
        }
    }
    //return;
    if(strcmp(n1->name,"Exp")==0){
        //return;
        if(strcmp(n2->name,"ASSIGNOP")==0){
    //Exp → Exp ASSIGNOP Exp 
           if(strcmp(n1->child->name,"ID")==0){
            //return;
             FieldList newlist1=serchvarname(n1->child->sID);
             //return;
             if(newlist1 !=NULL && newlist1->inter==NULL){
                newlist1->inter=newvariable();
             }
             //return;
             char*t1=newtemp();
             trans_Exp(n3,file,t1);
             //return;
             fprintf(file,"%s  :=  %s\n",newlist1->inter,t1);
             if(place!=NULL){
                fprintf(file,"%s  :=  %s\n",place,newlist1->inter);
             }
           }
           else{
            char*t1=newtemp();
            char*t2=newtemp();
            transaddr_Exp(n1,file,t1);
            trans_Exp(n3,file,t2);
            fprintf(file,"*%s := %s\n",t1,t2);
            if(place!=NULL){
                fprintf(file,"%s  :=  *%s\n",place,t1);
             }
           }
        }
        else if(strcmp(n2->name,"AND")==0||strcmp(n2->name,"OR")==0||strcmp(n2->name,"RELOP")==0){
    //| Exp AND Exp
    //| Exp OR Exp 
    //| Exp RELOP Exp 
    //return;
           char*label11=newlabel();
           char*label12=newlabel();
           if(place!=NULL){
            fprintf(file,"%s := #0\n",place);
            trans_Cond(root,label11,label12,file);
            fprintf(file,"LABEL %s :\n",label11);
            fprintf(file,"%s := #1\n",place);
            fprintf(file,"LABEL %s :\n",label12);
           }
        }
        else if(strcmp(n2->name,"PLUS")==0){
            //return;
            char*t1=newtemp();
            char*t2=newtemp();
            trans_Exp(n1,file,t1);
            trans_Exp(n3,file,t2);
            if(place!=NULL)
            fprintf(file,"%s := %s + %s\n",place,t1,t2);
        }

//| Exp PLUS Exp 
        else if(strcmp(n2->name,"MINUS")==0){
            //return;
            char*t1=newtemp();
            char*t2=newtemp();
            trans_Exp(n1,file,t1);
            trans_Exp(n3,file,t2);
            if(place!=NULL)
            fprintf(file,"%s := %s - %s\n",place,t1,t2);
        }
//| Exp MINUS Exp 
       else if(strcmp(n2->name,"STAR")==0){
        //return;
            char*t1=newtemp();
            char*t2=newtemp();
            trans_Exp(n1,file,t1);
            trans_Exp(n3,file,t2);
            if(place!=NULL)
            fprintf(file,"%s := %s * %s\n",place,t1,t2);
        }
//| Exp STAR Exp 
      else if(strcmp(n2->name,"DIV")==0){
        //return;
            char*t1=newtemp();
            char*t2=newtemp();
            trans_Exp(n1,file,t1);
            trans_Exp(n3,file,t2);
            if(place!=NULL)
            fprintf(file,"%s := %s / %s\n",place,t1,t2);
        }
//| Exp DIV Exp 
//| Exp LB Exp RB 
      else if(strcmp(n2->name,"LB")==0){
        //return;
        char*t4=newtemp();
        transaddr_Exp(n1,file,t4);
        char*t1=newtemp();
        trans_Exp(n3,file,t1);
            char*t2=newtemp();
            char*t3=newtemp();
            Type type12 = Exp(n1);
            int sizenum=4;
            if(type12!=NULL && type12->kind==ARRAY){
                sizenum=getSize(type12->u.array.elem);
            }
           fprintf(file,"%s := %s * #%d\n",t2,t1,sizenum);
           fprintf(file,"%s := %s + %s\n",t3,t4,t2);
           if(place!=NULL)
           fprintf(file,"%s := *%s\n",place,t3);
      }
      else if(strcmp(n2->name,"DOT")==0){
//| Exp DOT ID 
//return;
        char*t1=newtemp();
        transaddr_Exp(n1,file,t1);
        Type type1=Exp(n1);
        FieldList newlist1=type1->u.structures.structure;
        int size1=0;
        while(strcmp(newlist1->name,n3->sID)!=0){
            size1+=getSize(newlist1->type);
            newlist1=newlist1->tail;
        }
        if(size1==0){
            if(place!=NULL)
            fprintf(file,"%s := *%s\n",place,t1);
        }
        else{
            char* t2=newtemp();
            fprintf(file,"%s := %s + #%d\n",t2,t1,size1);
            if(place!=NULL)
            fprintf(file,"%s := *%s\n",place,t2);
        }
      }
    }
    
    else if(strcmp(n1->name,"NOT")==0){//| NOT Exp 
    //return;
       char*label11=newlabel();
       char*label12=newlabel();
           if(place!=NULL){
            fprintf(file,"%s := #0\n",place);
            trans_Cond(root,label11,label12,file);
            fprintf(file,"LABEL %s :\n",label11);
            fprintf(file,"%s := #1\n",place);
            fprintf(file,"LABEL %s :\n",label12);
           }
    }
//| LP Exp RP 
   else if(strcmp(n1->name,"LP")==0){
    //return;
     trans_Exp(n2,file,place);
   }
//| MINUS Exp 
else if(strcmp(n1->name,"MINUS")==0){
    //return;
    char*t1=newtemp();
    trans_Exp(n2,file,t1);
    if(place!=NULL){
        fprintf(file,"%s := #0 - %s\n",place,t1);
    }
}
else if(strcmp(n1->name,"ID")==0){
//| ID 
//return;
    if(n2==NULL){
        FieldList newlist1=serchvarname(n1->sID);
        if(newlist1->inter==NULL)
           newlist1->inter=newvariable();
        if(place!=NULL){
            fprintf(file,"%s := %s\n",place,newlist1->inter);
        }
    }
//| ID LP Args RP 
    else if(strcmp(n3->name,"Args")==0){
        //return;
        FieldList arglist =NULL;
        FieldList* arg = &arglist;
        trans_Args(n3,file,arg);
        if(strcmp(n1->sID,"write")==0){
            fprintf(file,"WRITE %s\n",arglist->inter);
            if(place!=NULL){
                fprintf(file,"%s := #0\n",place);
            }
        }
        else{
                    while(arglist!=NULL){
                    fprintf(file,"ARG %s\n",arglist->inter);
                    arglist = arglist->tail;
                }
                if(place==NULL){
                    fprintf(file,"%s := CALL %s\n",newtemp(),n1->sID);
                }
                else{
                    fprintf(file,"%s := CALL %s\n",place,n1->sID);
                }
        }
    }
//| ID LP RP
    else{
        if(strcmp(n1->sID,"read")==0){
            if(place!=NULL)
              fprintf(file,"READ %s\n",place);
        }
        else{
            if(place==NULL)
               fprintf(file,"%s := CALL %s\n",newtemp(),n1->sID);
            else
               fprintf(file,"%s := CALL %s\n",place,n1->sID);
        }
    } 
}
//| INT 
else if(strcmp(n1->name,"INT")==0){
    //return;
    if(place!=NULL){
            fprintf(file,"%s := #%d\n",place,n1->snum);
        }
}
//| FLOAT 
else if(strcmp(n1->name,"FLOAT")==0){
    //return;
    if(place!=NULL){
            fprintf(file,"%s := #%f\n",place,n1->snum1);
        }
}
}

void transaddr_Exp(Node* root,FILE*file,char* place){
    Node* n1=root->child;
    Node* n2 =n1 ->silbing;
    Node* n3 = NULL;
    if(n2!=NULL)
    {
        n3 = n2->silbing;
    }
    if(strcmp(n1->name,"ID")==0){
        FieldList newlist1=serchvarname(n1->sID);
        if(newlist1->inter==NULL)
           newlist1->inter=newvariable();
        if(place!=NULL){
            if(newlist1->flag1==0)
            fprintf(file,"%s := &%s\n",place,newlist1->inter);
            else
            fprintf(file,"%s := %s\n",place,newlist1->inter);
        }
    }
    else if(strcmp(n1->name,"Exp")==0){
        //| Exp LB Exp RB 
      if(strcmp(n2->name,"LB")==0){
        char*t4=newtemp();
        transaddr_Exp(n1,file,t4);
        char*t1=newtemp();
        trans_Exp(n3,file,t1);
            char*t2=newtemp();
            Type type12 = Exp(n1);
            int sizenum=4;
            if(type12!=NULL && type12->kind==ARRAY){
                sizenum=getSize(type12->u.array.elem);
            }
           fprintf(file,"%s := %s * #%d\n",t2,t1,sizenum);
           if(place!=NULL)
           fprintf(file,"%s := %s + %s\n",place,t4,t2);
      }
      else if(strcmp(n2->name,"DOT")==0){
//| Exp DOT ID 
        char*t1=newtemp();
        transaddr_Exp(n1,file,t1);
        Type type1=Exp(n1);
        FieldList newlist1=type1->u.structures.structure;
        int size1=0;
        while(strcmp(newlist1->name,n3->sID)!=0){
            size1+=getSize(newlist1->type);
            newlist1=newlist1->tail;
        }
        if(size1==0){
            if(place!=NULL)
            fprintf(file,"%s := %s\n",place,t1);
        }
        else{
            if(place!=NULL)
            fprintf(file,"%s := %s + #%d\n",place,t1,size1);
        }
      }
    }
}
void trans_Args(Node* root,FILE* file,FieldList* arg){
//Args → Exp COMMA Args 
//| Exp 
   Node*n1=root->child;
    char* t1 = newtemp();
    Type type12 = Exp(n1);
    if(type12!=NULL && (type12->kind==ARRAY || type12->kind==STRUCTURE))
    transaddr_Exp(n1,file,t1);
    else
    trans_Exp(n1,file,t1);
    FieldList arglist = *arg;
    if(arglist == NULL){
        arglist = (FieldList)malloc(sizeof(struct FieldList_));
        arglist->inter = (char*)malloc((strlen(t1)+1)*sizeof(char));
        strcpy(arglist->inter,t1);
        arglist->tail =NULL;
        *arg = arglist;
    }
    else{
        //printf("hello");
        FieldList arglist1 = (FieldList)malloc(sizeof(struct FieldList_));
        arglist1->inter = (char*)malloc((strlen(t1)+1)*sizeof(char));
        strcpy(arglist1->inter,t1);
        arglist1->tail =arglist;
        arglist = arglist1;
        *arg = arglist;
    }
   if(n1->silbing != NULL){
    trans_Args(n1->silbing->silbing,file,arg);
   }
}



