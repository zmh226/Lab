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
    init_obj(file);
    init_registers();
    init_varlist();
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

   //fprintf(file,"FUNCTION %s :\n",n1->sID);
   fprintf(file,"%s:\n",n1->sID);
   enter_func(file);
   clear_varlist();


   Type type1=serchfunc(n1->sID);
   int num1=type1->u.function.num_of_parameter;
   FieldList param1=type1->u.function.parameters;
   int num2=0;
   while(num1>0){
    param1->inter=newvariable();
    //fprintf(file,"PARAM %s\n",param1->inter);
    char* r1=deal_value(param1->inter,file);
    switch (num2)
    {
    case 0:{assign_from_const_reg(param1->inter,"$a0",file);break;}
    case 1:{assign_from_const_reg(param1->inter,"$a1",file);break;}
    case 2:{assign_from_const_reg(param1->inter,"$a2",file);break;}
    case 3:{assign_from_const_reg(param1->inter,"$a3",file);break;}
    
    default:
    {
        fprintf(file,"    sw %s,%d($fp)\n",r1,(num2-4)*4);
        Varible var1=new_var(param1->inter);
        fprintf(file,"    lw %s,-%d($fp)\n",r1,var1->offset);
        break;
    }       
    }
    num2++;
    //fprintf(file,"    ")
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
        //fprintf(file,"DEC %s %d\n",newlist1->inter,size1);
        new_arrayvar(newlist1->inter,size1);
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
    //exit_func(file);
     Node*n2=n1->silbing;
     if(strcmp(n2->child->name,"INT")==0){
        exit_func(file);
        //fprintf(file,"RETURN #%d\n",n2->child->snum);
        if(n2->child->snum==0)
        {
            fprintf(file,"    move $v0,$0\n");
            fprintf(file,"    jr $ra\n");
        }
        else
        {
            //char* t1=newtemp();
            //char* r1=deal_value(t1);
            fprintf(file,"    li $v0,%d\n",n2->child->snum);
            //fprintf(file,"    move $v0,%s\n",r1);
            fprintf(file,"    jr $ra\n");
        }
        
     }
     else if(strcmp(n2->child->name,"ID")==0 && n2->child->silbing==NULL){
        FieldList newlist1 = serchvarname(n2->child->sID);

        //fprintf(file,"RETURN %s\n",newlist1->inter);
        char* r1=deal_value(newlist1->inter,file);
        fprintf(file,"    move $v0,%s\n",r1);
        free_reg(r1);
        exit_func(file);
        fprintf(file,"    jr $ra\n");
     }
     else{
     char*t1=newtemp();
     trans_Exp(n1->silbing,file,t1);
     //fprintf(file,"RETURN %s\n",t1);
     char* r1=deal_value(t1,file);
     fprintf(file,"    move $v0,%s\n",r1);
     free_reg(t1);
     exit_func(file);
     fprintf(file,"    jr $ra\n");
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

       //fprintf(file,"LABEL %s :\n",label11);
       fprintf(file,"%s:\n",label11);

       trans_Stmt(n5,file);

       //fprintf(file,"LABEL %s :\n",label12);
       fprintf(file,"%s:\n",label12);

    }
    else{//| IF LP Exp RP Stmt ELSE Stmt 
       char* label11=newlabel();
       char* label12=newlabel();
       char* label13=newlabel();
       trans_Cond(n4,label11,label12,file);
       //fprintf(file,"LABEL %s :\n",label11);
       fprintf(file,"%s:\n",label11);

       trans_Stmt(n5,file);

       //fprintf(file,"GOTO %s\n",label13);
       fprintf(file,"    j %s\n",label13);

       //fprintf(file,"LABEL %s :\n",label12);
       fprintf(file,"%s:\n",label12);

       trans_Stmt(n3->silbing,file);
       //fprintf(file,"LABEL %s :\n",label13);
       fprintf(file,"%s:\n",label13);
    }
 }
 else if(strcmp(n1->name,"WHILE")==0){
//| WHILE LP Exp RP Stmt
    Node*n8=n1->silbing->silbing;
    Node*n9=n8->silbing->silbing;
    char* label11=newlabel();
    char* label12=newlabel();
    char* label13=newlabel();

    //fprintf(file,"LABEL %s :\n",label11);
    fprintf(file,"%s:\n",label11);

    trans_Cond(n8,label12,label13,file);

    //fprintf(file,"LABEL %s :\n",label12);
    fprintf(file,"%s:\n",label12);

    trans_Stmt(n9,file);

    //fprintf(file,"GOTO %s\n",label11);
    fprintf(file,"    j %s\n",label11);

    //fprintf(file,"LABEL %s :\n",label13);
    fprintf(file,"%s:\n",label13);
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

        //fprintf(file,"IF %s %s %s GOTO %s\n",t1,n2->sID,t2,label_true);
        {   char* v1=deal_value(t1,file);
            char* v2=deal_value(t2,file);

            if(strcmp(n2->sID,"==")==0)
            {
                fprintf(file,"    beq %s,%s,%s\n",v1,v2,label_true);
            }
            if(strcmp(n2->sID,"!=")==0)
            {
                fprintf(file,"    bne %s,%s,%s\n",v1,v2,label_true);
            }
            if(strcmp(n2->sID,">")==0)
            {
                fprintf(file,"    bgt %s,%s,%s\n",v1,v2,label_true);
            }
            if(strcmp(n2->sID,"<")==0)
            {
                fprintf(file,"    blt %s,%s,%s\n",v1,v2,label_true);
            }
            if(strcmp(n2->sID,">=")==0)
            {
                fprintf(file,"    bge %s,%s,%s\n",v1,v2,label_true);
            }
            if(strcmp(n2->sID,"<=")==0)
            {
                fprintf(file,"    ble %s,%s,%s\n",v1,v2,label_true);
            }
            free_reg(v1);free_reg(v2);

            
        }

        //fprintf(file,"GOTO %s\n",label_false);
        fprintf(file,"    j %s\n",label_false);
    }
    //| NOT Exp
    else if(strcmp(n1->name,"NOT")==0){
        trans_Cond(n2,label_false,label_true,file);
    }
    //Exp AND Exp
    else if(n2!=NULL && strcmp(n2->name,"AND")==0){
        char*label11=newlabel();
        trans_Cond(n1,label11,label_false,file);

        //fprintf(file,"LABEL %s :\n",label11);
        fprintf(file,"%s:\n",label11);

        trans_Cond(n3,label_true,label_false,file);
    }
    //Exp OR Exp 
    else if(n2!=NULL && strcmp(n2->name,"OR")==0){
        char*label11=newlabel();
        trans_Cond(n1,label_true,label11,file);

        //fprintf(file,"LABEL %s :\n",label11);
        fprintf(file,"%s:\n",label11);


        trans_Cond(n3,label_true,label_false,file);
    }
    else{
        char*t1=newtemp();
        trans_Exp(root,file,t1);
        //fprintf(file,"IF %s != #0 GOTO %s\n",t1,label_true);
        {
            char* v1=deal_value(t1,file);
            fprintf(file,"    bne %s,$0,%s\n",v1,label_true);
            free_reg(v1);
        }

        //fprintf(file,"GOTO %s\n",label_false);
        fprintf(file,"    j %s\n",label_false);

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
             //fprintf(file,"%s  :=  %s\n",newlist1->inter,t1);
            //  char*r1=deal_value(t1);
            //  char*r2=deal_value(newlist1->inter);
            //  fprintf(file,"    move %s,%s\n",r2,r1);
             assign_v(newlist1->inter,t1,file);

             if(place!=NULL){
                //fprintf(file,"%s  :=  %s\n",place,newlist1->inter);
                //fprintf(file,"    move %s,%s\n",r1,r2);
                assign_v(t1,newlist1->inter,file);
             }
           }
           else{
            char*t1=newtemp();
            char*t2=newtemp();
            transaddr_Exp(n1,file,t1);
            trans_Exp(n3,file,t2);

            //fprintf(file,"*%s := %s\n",t1,t2);
            // char*r1=deal_value(t1,file);
            // char*r2=deal_value(t2,file);
            // fprintf(file,"    sw %s,0(%s)\n",r1,r2);
            assign_addr(t2,t1,1,file);

            if(place!=NULL){
                // fprintf(file,"%s  :=  *%s\n",place,t1);
                // char* r3=deal_value(place,file);
                // fprintf(file,"    lw %s,0(%s)\n",r3,r1);
                assign_addr(place,t1,2,file);
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
            //fprintf(file,"%s := #0\n",place);
            // char* r1=deal_value(place);
            // fprintf(file,"    li %s,0\n",r1);
            assign_i(place,0,file);



            trans_Cond(root,label11,label12,file);

            //fprintf(file,"LABEL %s :\n",label11);
            fprintf(file,"%s:\n",label11);

            //fprintf(file,"%s := #1\n",place);
            //fprintf(file,"    li %s,1\n",r1);
            assign_i(place,1,file);



            //fprintf(file,"LABEL %s :\n",label12);
            fprintf(file,"%s:\n",label12);

           }
        }
        else if(strcmp(n2->name,"PLUS")==0){
            //return;
            char*t1=newtemp();
            char*t2=newtemp();
            trans_Exp(n1,file,t1);
            trans_Exp(n3,file,t2);
            if(place!=NULL)
            {
                //fprintf(file,"%s := %s + %s\n",place,t1,t2);
                // char* rp=deal_value(place);
                // char* r1=deal_value(t1);
                // char* r2=deal_value(t2);
                // fprintf(file,"    add %s,%s,%s\n",rp,r1,r2);
                operation2(place,t1,t2,'+',file);


            }
            
        }

//| Exp PLUS Exp 
        else if(strcmp(n2->name,"MINUS")==0){
            //return;
            char*t1=newtemp();
            char*t2=newtemp();
            trans_Exp(n1,file,t1);
            trans_Exp(n3,file,t2);
            if(place!=NULL)
            {
                //fprintf(file,"%s := %s - %s\n",place,t1,t2);
                // char* rp=deal_value(place);
                // char* r1=deal_value(t1);
                // char* r2=deal_value(t2);
                // fprintf(file,"    sub %s,%s,%s\n",rp,r1,r2);
                operation2(place,t1,t2,'-',file);
            }
        }
//| Exp MINUS Exp 
       else if(strcmp(n2->name,"STAR")==0){
        //return;
            char*t1=newtemp();
            char*t2=newtemp();
            trans_Exp(n1,file,t1);
            trans_Exp(n3,file,t2);
            if(place!=NULL)
            {
                //fprintf(file,"%s := %s * %s\n",place,t1,t2);
                // char* rp=deal_value(place);
                // char* r1=deal_value(t1);
                // char* r2=deal_value(t2);
                // fprintf(file,"    mul %s,%s,%s\n",rp,r1,r2);
                operation2(place,t1,t2,'*',file);
            }
        }
//| Exp STAR Exp 
      else if(strcmp(n2->name,"DIV")==0){
        //return;
            char*t1=newtemp();
            char*t2=newtemp();
            trans_Exp(n1,file,t1);
            trans_Exp(n3,file,t2);
            if(place!=NULL)
            {
                //fprintf(file,"%s := %s / %s\n",place,t1,t2);
                // char* rp=deal_value(place);
                // char* r1=deal_value(t1);
                // char* r2=deal_value(t2);
                // fprintf(file,"    div %s,%s\n",r1,r2);
                // fprintf(file,"    mflo %s\n",rp);
                operation2(place,t1,t2,'/',file);
            
            }
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
           //fprintf(file,"%s := %s * #%d\n",t2,t1,sizenum);
           char* t5=newtemp();
           assign_i(t5,sizenum,file);
           operation2(t2,t1,t5,'*',file);

           //fprintf(file,"%s := %s + %s\n",t3,t4,t2);
           operation2(t3,t4,t2,'+',file);

           if(place!=NULL)
           //fprintf(file,"%s := *%s\n",place,t3);
           assign_addr(place,t3,2,file);

      }
      else if(strcmp(n2->name,"DOT")==0){
//| Exp DOT ID 样例中不会出现结构体类型，不管
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
            //fprintf(file,"%s := #0\n",place);
            // char* r1=deal_value(place);
            // fprintf(file,"    li %s,0\n",r1);
            assign_i(place,0,file);


            trans_Cond(root,label11,label12,file);

            //fprintf(file,"LABEL %s :\n",label11);
            fprintf(file,"%s:\n",label11);

            //fprintf(file,"%s := #1\n",place);
            // char* r2=deal_value(place);
            // fprintf(file,"    li %s,1\n",r2);
            assign_i(place,1,file);

            //fprintf(file,"LABEL %s :\n",label12);
            fprintf(file,"%s:\n",label12);
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
        //fprintf(file,"%s := #0 - %s\n",place,t1);
        // char *rp=deal_value(place);
        // char *r1=deal_value(t1);
        // fprintf(file,"    sub %s,$0,%s\n",place,t1);
        operation1(place,t1,file);
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
            //fprintf(file,"%s := %s\n",place,newlist1->inter);
            // char* r1=deal_value(place);
            // char* r2=deal_value(newlist1->inter);
            // fprintf(file,"    move %s,%s\n",r1,r2);
            assign_v(place,newlist1->inter,file);

        }
    }
//| ID LP Args RP 
    else if(strcmp(n3->name,"Args")==0){
        //return;
        FieldList arglist =NULL;
        FieldList* arg = &arglist;
        trans_Args(n3,file,arg);
        if(strcmp(n1->sID,"write")==0){
            //fprintf(file,"WRITE %s\n",arglist->inter);
            // if(place!=NULL){
            //     fprintf(file,"%s := #0\n",place);
            // }
            char* v1=deal_value(arglist->inter,file);
            // reallocate("$v0",file);
            // reallocate("$v1",file);
            // reallocate("$a0",file);
            // reallocate("$a1",file);
            // reallocate("$a2",file);
            // reallocate("$a3",file);
            fprintf(file,"    move $a0,%s\n",v1);
            free_reg(v1);
            fprintf(file,"    addi $sp, $sp, -4\n");
            fprintf(file,"    sw $ra, 0($sp)\n");
            fprintf(file,"    jal write\n");
            fprintf(file,"    lw $ra, 0($sp)\n");
            fprintf(file,"    addi $sp, $sp, 4\n");
            if(place==NULL){
                    char* t1=newtemp();
                    assign_from_const_reg(t1,"$v0",file);
                }
                else{
                    assign_from_const_reg(place,"$v0",file);
                }

        }
        else{
            int num=0;
            FieldList arglist_temp=arglist;
                    while(arglist_temp!=NULL)
                    {
                        num++;
                        arglist_temp = arglist_temp->tail;
                    }
                    if(num>4)
                    fprintf(file,"    subu $sp, $sp,%d\n",4*num-16);
                    int i=num-1;
                    while(arglist!=NULL)
                    {
                        //fprintf(file,"ARG %s\n",arglist->inter);
                        char* v1=deal_value(arglist->inter,file);
                        if(i<=3) fprintf(file,"    move $a%d,%s\n",i,v1);
                        else
                        {
                            fprintf(file,"    sw %s,%d($sp)\n",v1,4*(i-4));
                        }
                        free_reg(v1);
                        arglist = arglist->tail;
                        i--;
                    }
                    fprintf(file,"    jal %s\n",n1->sID);

                    if(num>4) fprintf(file,"    addi $sp, $sp,%d\n",4*num-16);
                    // fprintf(file,"    addi $sp, $sp, -4\n");
                    // fprintf(file,"    sw $ra, 0($sp)\n");
                    // fprintf(file,"    jal write\n");
                    // fprintf(file,"    lw $ra, 0($sp)\n");


                if(place==NULL){

                    //fprintf(file,"%s := CALL %s\n",newtemp(),n1->sID);
                    char* t1=newtemp();
                    // char* r1=deal_value(t1,file);
                    // fprintf(file,"    move %s,$v0\n",r1);
                    assign_from_const_reg(t1,"$v0",file);

                }
                else{
                    //fprintf(file,"%s := CALL %s\n",place,n1->sID);
                    // char* r1=deal_value(place);
                    // fprintf(file,"    move %s,$v0\n",r1);
                    assign_from_const_reg(place,"$v0",file);
                }
        }
    }
//| ID LP RP
    else{
        if(strcmp(n1->sID,"read")==0){
            if(place!=NULL)
            {
              //fprintf(file,"READ %s\n",place);
              //char* v1=deal_value(place,file);
            //   reallocate("$v0",file);
            //   reallocate("$v1",file);
            //   reallocate("$a0",file);
            //   reallocate("$a1",file);
            //   reallocate("$a2",file);
            //   reallocate("$a3",file);
              fprintf(file,"    addi $sp, $sp, -4\n");
              fprintf(file,"    sw $ra, 0($sp)\n");
              fprintf(file,"    jal read\n");
              fprintf(file,"    lw $ra, 0($sp)\n");
              fprintf(file,"    addi $sp, $sp, 4\n");
              //fprintf(file,"    move %s,$v0\n",v1);
              assign_from_const_reg(place,"$v0",file);
            }
        }
        else{
            fprintf(file,"    addi $sp, $sp, -4\n");
            fprintf(file,"    sw $ra, 0($sp)\n");
            fprintf(file,"    jal %s\n",n1->sID);
            fprintf(file,"    lw $ra, 0($sp)\n");
            fprintf(file,"    addi $sp, $sp, 4\n");
            if(place==NULL)
               //fprintf(file,"%s := CALL %s\n",newtemp(),n1->sID);
               ;
            else
               assign_from_const_reg(place,"$v0",file);
        }
    } 
}
//| INT 
else if(strcmp(n1->name,"INT")==0){
    //return;
    if(place!=NULL){
            //fprintf(file,"%s := #%d\n",place,n1->snum);
            // char* r1=deal_value(place);
            // fprintf(file,"    li %s,%d\n",r1,n1->snum);
            assign_i(place,n1->snum,file);

        }
}
//| FLOAT 
else if(strcmp(n1->name,"FLOAT")==0){
    //return;
    if(place!=NULL){
            fprintf(file,"%s := #%f\n",place,n1->snum1);//样例中不会出现浮点数，不管
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
            //fprintf(file,"%s := &%s\n",place,newlist1->inter);
            {
                int arr_off=get_offset(newlist1->inter);
                char* r1=deal_value(place,file);

                fprintf(file,"    addi %s,$fp,-%d\n",r1,arr_off);
                Varible var1=search_var(place);
                if(var1==NULL) {//如果变量不存在，栈中开一块新区域存该变量的值，并产生一个新的变量
                    var1=new_var(place);
                    fprintf(file,"    subu $sp,$sp,4\n");    
                }
                fprintf(file,"    sw %s,-%d($fp)\n",r1,var1->offset);//无论name1对应的变量是否存在，在本次运算中值改变了，因此需要将其写回栈中
                free_reg(r1);
            }
            else
            //fprintf(file,"%s := %s\n",place,newlist1->inter);
            assign_v(place,newlist1->inter,file);
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

           //fprintf(file,"%s := %s * #%d\n",t2,t1,sizenum);
           char *t3=newtemp();
           assign_i(t3,sizenum,file);
           operation2(t2,t1,t3,'*',file);


           if(place!=NULL)
           //fprintf(file,"%s := %s + %s\n",place,t4,t2);
           operation2(place,t4,t2,'+',file);
      }
      else if(strcmp(n2->name,"DOT")==0){
//| Exp DOT ID 样例中不会出现结构体类型，不管
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



