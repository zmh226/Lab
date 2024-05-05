#include"semantic.h"
extern int transflag;

#define DEBUG

void Program_init(Node* root)
{
    init_hashtable();
    init_stacktable();
    push_stacktable();
    if(root==NULL) return;
    Type type1 = (Type)malloc(sizeof(struct Type_));
    Type type2 = (Type)malloc(sizeof(struct Type_));
    type1->kind=FUNCTION;
    type2->kind=BASIC;
    type2->u.basic=0;
    type1->u.function.return_parameter=type2;
    type1->u.function.num_of_parameter=0;
    type1->u.function.parameters=NULL;
    FieldList func = init_FieldList_("read",type1);
    HashNode newnode = init_HashNode_(func,0);
    insert0_hashnode(newnode);
    Type type3 = (Type)malloc(sizeof(struct Type_));
    type3->kind=FUNCTION;
    type3->u.function.return_parameter=type2;
    type3->u.function.num_of_parameter=1;
    FieldList param = init_FieldList_(NULL,type2);
    type3->u.function.parameters=param;
    FieldList func1 = init_FieldList_("write",type3);
    HashNode newnode1 = init_HashNode_(func1,0);
    insert0_hashnode(newnode1);
    //printf("1");
    ExtDefList(root->child);
}

void ExtDefList(Node* root)
{
    if(root==NULL) return;
    //printf("%d\n",root->sline);
    if(root->child != NULL){
       ExtDef(root->child);
       ExtDefList(root->child->silbing);
    }
}

void ExtDef(Node* root)
{
    if(root==NULL) return;
    Node* t1=root->child;
    Node* t2=NULL;
    Node* t3=NULL;
    if(t1!=NULL){
        t2 = t1->silbing;
    }
    if(t2!=NULL){
        t3 = t2->silbing;
    }
    Type type1=NULL;
    //ExtDef → Specifier ExtDecList SEMI| Specifier SEMI | Specifier FunDec CompSt 
    if(t1!= NULL){
       type1 = Specifier(t1);
    }
    if(strcmp(t2->name,"ExtDecList")==0)
    {
        ExtDecList(t2,type1); 
    }
    else if(strcmp(t2->name,"FunDec")==0)
    {
       FunDec(t2,type1);
       CompSt(t3,type1);
    }

}
void ExtDecList(Node* root,Type type){//ExtDecList → VarDec | VarDec COMMA ExtDecList
     Node* t1 = root->child;
     if(type==NULL) return;
     while(t1 != NULL){
        FieldList newi=VarDec(t1,type);
        int false1= Determin(newi->name,newi->type);
        if(false1==3){
            printf("Error type 3 at Line %d: Redifned  Variable %s\n",t1->sline,newi->name);
        }
        else if(false1==0){
            HashNode newnode= init_HashNode_(newi,0);
            insert0_hashnode(newnode);
        }
        if(t1->silbing !=NULL && t1->silbing->silbing != NULL)
           t1 = t1->silbing->silbing->child;
        else
           t1=NULL;
     }
     return;
}

void FunDec(Node* root,Type type){//FunDec → ID LP VarList RP | ID LP RP 
    //printf("FUC LINE: %d\n",root->sline);
    Node*t1=root->child;
    Type type1=(Type)malloc(sizeof(struct Type_));
    type1->kind=FUNCTION;
    type1->u.function.return_parameter=type;
    int flag= Determin(t1->sID,type1);
    if(flag==4){
        printf("Error type 4 at Line %d: Redifned  Function %s\n",t1->sline,t1->sID);
    }
    if(strcmp(t1->silbing->silbing->name,"RP")==0){
        type1->u.function.num_of_parameter=0;
        type1->u.function.parameters=NULL;
        //push_stacktable();
    }
    else if(strcmp(t1->silbing->silbing->name,"VarList")==0){
        type1->u.function.num_of_parameter=0;
        type1->u.function.parameters=NULL;
        Node*t2=t1->silbing->silbing;
        Node*t3=t2->child;//VarList → ParamDec COMMA VarList | ParamDec 
        //push_stacktable();
        while(t3!=NULL){
            type1->u.function.num_of_parameter+=1;
            FieldList param = ParamDec(t3);
            if(param==NULL)  
                type1->u.function.num_of_parameter-=1;
            if(type1->u.function.parameters==NULL){
                type1->u.function.parameters=param;
            }
            else{
                FieldList temp1 = type1->u.function.parameters;
                FieldList temp2 = type1->u.function.parameters;
                while(temp1!=NULL){
                    temp2=temp1;
                    temp1=temp1->tail;
                }
                temp2->tail=param;
            }
            if(t3->silbing == NULL)
                t3=NULL;
            else
                t3=t3->silbing->silbing->child;
        }
    }
    if(flag==0){
         FieldList func = init_FieldList_(t1-> sID,type1);
         HashNode newnode = init_HashNode_(func,0);
         insert0_hashnode(newnode);
         //printf("hello\n");
    }
}

FieldList ParamDec(Node*root){//ParamDec → Specifier VarDec 
     Node*t1 = root->child;
     Node*t2= t1-> silbing;
     Type type0 = (Type)malloc(sizeof(struct Type_));
     type0->kind=BASIC;
     int flag = Determin(t2->child->sID,type0);
     if(flag==3){
        printf("Error type 3 at Line %d: Redifned  Variable %s\n",t2->child->sline,t2->child->sID);
     }
     Type type1 = Specifier(t1);
     if(type1 ==NULL)
       return NULL;
    FieldList newlist = VarDec(t2,type1);
    newlist->flag1=1;
    HashNode newnode=init_HashNode_(newlist,DEPTH);
    insert_hashnode(newnode);
    return newlist;
}

void CompSt(Node* root,Type ret_type)
{//由于函数形参也是函数的局部作用域，因此Compst中将不压栈，请调用时自己看情况是否压栈！但语句块分析完毕后退栈应当在Compst中进行
    // printf("compst line%d\n",root->sline);
    // printf("%s\n",root->child->silbing->name);
    Node* t1=root->child->silbing;
    Node* t2=NULL;
    Node* t3=NULL;
    // if(t1==NULL) printf("empty point t1\n");
    //printf("%s\n",t1->name);
    if(t1->silbing!=NULL)
    {
        t2=t1->silbing;
        if(strcmp(t1->name,"DefList")==0) DefList(t1);
        else if(strcmp(t1->name,"StmtList")==0) StmtList(t1,ret_type);
        if (t2->silbing!=NULL)
        {
            if(strcmp(t2->name,"StmtList")==0) StmtList(t2,ret_type);
        }
    }
    // printf("before pop\n");
    //printf("DEPTH:%d\n",DEPTH);    
    //pop_stacktable();  
}

void StmtList(Node* root,Type ret_type)
{
    if(root==NULL) return;
    //printf("stmtlistline: %d\n",root->sline);
    Node*t1=root->child;   
    if(t1!=NULL)
    {
        Stmt(t1,ret_type);
        Node*t2=t1->silbing;
        if(t2!=NULL) StmtList(t2,ret_type);
    }
}

void Stmt(Node* root,Type ret_type)
{
    Node* t1=root->child;
    Node* t2=t1->silbing;
    Node* t3=NULL;
    Node* t5=NULL;
    Node* t7=NULL;
    //printf("stmt line:%d\n",root->sline);
    if(t2!=NULL)
    {
        t3=t2->silbing;
        if(t3!=NULL)
        {
            if(t3->silbing!=NULL)
            {
            t5=t3->silbing->silbing;
            if(t5->silbing!=NULL) t7=t5->silbing->silbing; 
            }
        }
    }

    if(strcmp(t1->name,"CompSt")==0)
    {
        //push_stacktable();
        //printf("haha\n");
        CompSt(t1,ret_type);
    }
    else if(strcmp(t1->name,"RETURN")==0)
    {
        //printf("RETIRN LINE %d\n",t1->sline);
        t2=t1->silbing;
        if(typeEqual(Exp(t2),ret_type)!=1)
        {
           printf("Error type 8 at Line %d: Return type mismatch\n",t1->sline);
        }
    }
    else if(strcmp(t1->name,"WHILE")==0)
    {
        Exp(t3);
        Stmt(t5,ret_type);       
    }
    else if(strcmp(t1->name,"IF")==0)
    {
        Exp(t3);
        //printf("haha1\n");
        Stmt(t5,ret_type);
        if(t7!=NULL)
        {
            Stmt(t7,ret_type);
        }
    }
    else if(strcmp(t1->name,"Exp")==0)
    {
        Exp(t1);
    }
}

void DefList(Node*root){// DefList -> Def DefList| (empty)
     Node*t1=root->child;
     if(t1==NULL) return;
     Node*t2=t1->silbing;
     if(t1!=NULL){
        Def(t1);
        if(t2!=NULL)
           DefList(t2);
     }
}


void Def(Node* root){//Def → Specifier DecList SEMI
   Node* t1=root->child;
   Node* t2=t1->silbing;
   Type type1 = Specifier(t1);
   if(type1 == NULL) return;
   if(t2!=NULL)
      DecList(t2,type1);
}

void DecList(Node*root,Type type){//DecList → Dec | Dec COMMA DecList
   Node*t1=root->child;
   if(t1!=NULL){
    Dec(t1,type);
    if(t1->silbing != NULL)
       DecList(t1->silbing->silbing,type);
   }
}

void Dec(Node*root,Type type){//Dec → VarDec | VarDec ASSIGNOP Exp 
   Node*t1=root->child;
   FieldList newlist = VarDec(t1,type);
   int error_node=Determin(newlist->name,type);
   if(error_node==3)
   {
    printf("Error type 3 at Line %d: Redifned  Variable %s\n",t1->sline,newlist->name);return;
   }
   HashNode newnode= init_HashNode_(newlist,DEPTH);
   insert_hashnode(newnode);
   if(t1->silbing != NULL){
    if(typeEqual(type,Exp(t1->silbing->silbing))==0)
       printf("Error type 5 at Line %d: ASSIGNOP ERROR\n",t1->sline);
   }
}

Type Exp(Node*root){//若返回NULL，表示不能加减乘除
   //Exp → Exp ASSIGNOP Exp 
//| Exp AND Exp 
//| Exp OR Exp 
//| Exp RELOP Exp 
//| Exp PLUS Exp 
//| Exp MINUS Exp 
//| Exp STAR Exp 
//| Exp DIV Exp 
//| LP Exp RP 
//| MINUS Exp 
//| NOT Exp 
//| ID LP Args RP 
//| ID LP RP 
//| Exp LB Exp RB 
//| Exp DOT ID 
//| ID 
//| INT 
//| FLOAT
   
   Node*t1=root->child;
   //if(t1->child!=NULL)printf("has child %s %d %s\n",t1->child->name,t1->sline,t1->child->sID);
    // if(t1->child!=NULL)printf("%s %d no child\n",t1->name,t1->sline);
   Node*t2=t1->silbing;
   Node*t3=NULL;
   Node*t4=NULL;
   if(t2!=NULL){
    t3=t2->silbing;
   }
   if(t3!=NULL){
    t4=t3->silbing;
   }
   //ID
   if(strcmp(t1->name,"ID")==0)
   {
    if(t2==NULL) 
    {
        Type temp=serchvar(t1->sID);
        //printf("1");
        if(temp==NULL)
        {
            printf("Error type 1 at Line %d:The variable %s is undefined when used.\n",t1->sline,t1->sID);
            return NULL; 
        }     
        else return serchvar(t1->sID);
    }
    else 
    {
        // printf("use func line:%d",t1->sline);
        Type temp=serchfunc(t1->sID);
        Type temp2=serchvar(t1->sID);
        if(temp2!=NULL &&temp2->kind!=FUNCTION)
        {
            printf("Error type 11 at Line %d: Use function call operator on a regular variable.\n",t1->sline);
            return NULL;
        }
        
        if(temp==NULL)
        {
            printf("Error type 2 at Line %d: The function is undefined when called.\n",t1->sline);
            return NULL;
        }
        
        else
        {
            if(strcmp(t3->name,"RP")==0)
            {
               if(temp->u.function.num_of_parameter!=0)
               {
                printf("Error type 9 at Line %d: The number or type of actual parameters involved in function %s calls do not match",t1->sline,t1->sID);
                return NULL;
               }
               else return temp->u.function.return_parameter;
            }
            else
            {
                Node* t3_temp=t3->child;
                Type type1=Exp(t3_temp);
                FieldList para=temp->u.function.parameters;
                Type type2=para->type;
                while(t3_temp->silbing!=NULL&&para->tail!=NULL)
                {
                    if(type1==NULL||type2==NULL||typeEqual(type1,type2)!=1)
                    {
                        printf("Error type 9 at Line %d: The number or type of actual parameters involved in function %s calls do not match\n",t1->sline,t1->sID);
                        return temp->u.function.return_parameter;
                    }
                    t3_temp=t3_temp->silbing->silbing->child;
                    type1=Exp(t3_temp);
                    para=para->tail;
                    type2=para->type;              
                }
                if(t3_temp->silbing==NULL&&para->tail==NULL)//compare last
                {
                    if(type1==NULL||type2==NULL||typeEqual(type1,type2)!=1)
                    {
                        printf("Error type 9 at Line %d: The number or type of actual parameters involved in function %s calls do not match\n",t1->sline,t1->sID);
                    }
                    return temp->u.function.return_parameter;
                }//number not equal
                else
                {
                    printf("Error type 9 at Line %d: The number or type of actual parameters involved in function %s calls do not match\n",t1->sline,t1->sID);
                    return temp->u.function.return_parameter;
                } 
            }
        }
    }
    
   }
   //INT
   else if(strcmp(t1->name,"INT")==0){
    Type type1=(Type)malloc(sizeof(struct Type_));
    type1->kind=BASIC;
    type1->u.basic=0;
    return type1;
   }
   //FLOAT
   else if(strcmp(t1->name,"FLOAT")==0){
    Type type1=(Type)malloc(sizeof(struct Type_));
    type1->kind=BASIC;
    type1->u.basic=1;
    return type1;
   }
   //Exp ...
   else if(strcmp(t1->name,"Exp")==0){
    // printf("in exp turn%s %d\n",t1->name,t1->sline);
    if(strcmp(t2->name,"LB")==0){//Exp LB Exp RB 
        if(Exp(t3)==NULL || !(Exp(t3)->kind==BASIC && Exp(t3)->u.basic==0)){
            printf("Error type 12 at Line %d: array operator error\n",t3->sline);
        }
        if(Exp(t1)==NULL || Exp(t1)->kind != ARRAY){
            printf("Error type 10 at Line %d: not array \n",t1->sline);
            return NULL;
        }
        return Exp(t1)->u.array.elem;
    }
    else if(strcmp(t2->name,"DOT")==0){//Exp DOT ID
       //printf("1");
       if(Exp(t1)==NULL || Exp(t1)->kind != STRUCTURE){
            printf("Error type 13 at Line %d: not structure \n",t1->sline);
            return NULL;
        }
        Type temp1=Exp(t1);
        //printf("1");
        Type res=serchstructval(temp1,t3->sID);
        if(res==NULL){
            printf("Error type 14 at Line %d: undefined structure val\n",t3->sline);
            return NULL;
        }
        return res;
    }
    else if(strcmp(t2->name,"ASSIGNOP")==0)//Exp ASSIGNOP Exp 
    {
        // printf("%s %d\n",t1->name,t1->sline);
        Node* t1_child1=t1->child;
        Node* t1_child2=t1_child1->silbing;
        if(strcmp(t1_child1->name,"ID")!=0 && strcmp(t1_child1->name,"Exp")!=0)
        {
            printf("Error type 6 at Line %d: An expression with only a right value appears to the left of the assignment number\n",t2->sline);
            return NULL;
        }
        else if(strcmp(t1_child1->name,"ID")==0)
        {
            if(t1_child2!=NULL) 
            {
                printf("Error type 6 at Line %d: An expression with only a right value appears to the left of the assignment number\n",t2->sline);
                return NULL;
            }
        }
        else if(strcmp(t1_child1->name,"Exp")==0)
        {
            if(strcmp(t1_child2->name,"LB")&&strcmp(t1_child2->name,"RB")&&strcmp(t1_child2->name,"DOT"))
            {
                printf("Error type 6 at Line %d: An expression with only a right value appears to the left of the assignment number\n",t2->sline);
                return NULL;
            }
        }
        Type temp1=Exp(t1);
        Type temp2=Exp(t3);
        // printf("1kian%d\n",temp1->kind);
        // printf("2kind%d\n",temp2->kind);
        if(typeEqual(temp1,temp2)==1) 
        {
            //printf("type eqa line:%d",root->sline);
            return temp1;
            
        }
        else 
        {
            printf("Error type 5 at Line %d: types on both sides of the assignment number do not match\n",t2->sline);
            return NULL;
        }
    }
    else if(strcmp(t2->name,"PLUS")==0||strcmp(t2->name,"MINUS")==0||strcmp(t2->name,"STAR")==0||strcmp(t2->name,"DIV")==0)
    {
        Type temp1=Exp(t1),temp2=Exp(t3);
        //printf("%d\n",t1->sline);
        if(typeEqual(temp1,temp2)==1&&temp1->kind==BASIC) return temp1;
        else {
            printf("Error type 7 at Line %d: Operand type mismatch or operand type mismatch with operator\n",t2->sline);
        }
        return NULL;
    }
    else if(strcmp(t2->name,"AND")==0||strcmp(t2->name,"OR")==0){
        Type temp1=Exp(t1),temp2=Exp(t3);
        //printf("%d\n",t1->sline);
        if(typeEqual(temp1,temp2)==1&&temp1->kind==BASIC&&temp1->u.basic==0) return temp1;
        else {
            printf("Error type 7 at Line %d: Operand type mismatch or operand type mismatch with operator\n",t2->sline);
        }
        return NULL;
    }
    else if(strcmp(t2->name,"RELOP")==0){
        Type temp1=Exp(t1),temp2=Exp(t3);
        Type typenew = (Type)malloc(sizeof(struct Type_));
        typenew->kind = BASIC;
        typenew->u.basic = 0;
        //printf("%d\n",t1->sline);
        if(typeEqual(temp1,temp2)==1&&temp1->kind==BASIC) return typenew;
        else {
            printf("Error type 7 at Line %d: Operand type mismatch or operand type mismatch with operator\n",t2->sline);
        }
        return NULL;
    }
  }
  else if(strcmp(t1->name,"MINUS")==0||strcmp(t1->name,"NOT")==0)
  {
    Type temp=Exp(t2);
    if(temp!=NULL && temp->kind==BASIC) return temp;
    else printf("Error type 7 at Line %d: Operand type mismatch or operand type mismatch with operator \n",t2->sline);
    return NULL;
  }
  else if(strcmp(t1->name,"LP")==0){
    return Exp(t2);
  }
}

Type Specifier(Node* root){
    //Specifier → TYPE | StructSpecifier
    Node*t1 = root -> child;
    // printf("Spec line: %d\n",root->sline);
    Type type1 = (Type)malloc(sizeof(struct Type_));
    if(strcmp(t1->name,"TYPE")==0){
        type1->kind=BASIC;
        if(strcmp(t1->sID,"int")==0){
           type1->u.basic=0;
           
           }
        else if(strcmp(t1->sID,"float")==0){
           type1->u.basic=1;
           
        }
        return type1;
    }
    else if(strcmp(t1->name,"StructSpecifier")==0){//StructSpecifier → STRUCT OptTag LC DefList RC | STRUCT Tag 
        type1->kind = STRUCTURE;
        
        Node*t2=t1->child->silbing;
        if(strcmp(t2->name,"OptTag")==0){
            Node*t3=t2->child;
            
            Type type2 = (Type)malloc(sizeof(struct Type_));
            type2->kind = DEFINITION;
            int how=Determin(t3->sID,type2);
            if(how == 16){
                printf("Error type 16 at Line %d: Redifned Structure %s\n",t3->sline,t3->sID);
                return NULL;
            }
            else if(how == 0){
                type1->u.structures.name=(char*)malloc(sizeof(char)*(strlen(t3->sID)+1));
                strcpy(type1->u.structures.name,t3->sID);
                Node*t4=t2->silbing->silbing;
                if(strcmp(t4->name,"DefList")==0){
                    
                    type1->u.structures.structure=NULL;
                    type2->u.definition=NULL;
                    Node*t5 = t4;//DefList → Def DefList | empty
                    while(t5!=NULL){
                        Node*t6=t5->child->child;//Def → Specifier DecList SEMI 
                        Type typenew0=Specifier(t6);
                        Node*t7=t6->silbing;//DecList
                        while(t7!=NULL){//DecList → Dec | Dec COMMA DecList 
                             Node*t8 = t7->child->child;//t8代表VarDec     Dec → VarDec | VarDec ASSIGNOP Exp 
                             if(t8->silbing !=NULL){
                                printf("Error type 15 at Line %d:wrong initialization\n",t8->sline);
                             }
                             FieldList newinit = VarDec(t8,typenew0);
                             if(type1->u.structures.structure==NULL){
                                type1->u.structures.structure=newinit;
                             }
                             else{
                                FieldList now1=type1->u.structures.structure;
                                FieldList now2=type1->u.structures.structure;
                                int flag0=0;
                                while(now1!=NULL){
                                    if(strcmp(now1->name,newinit->name)==0){
                                        flag0=1;
                                        printf("Error type 15 at Line %d: Redifned field %s\n",t8->sline,newinit->name);
                                        break;
                                    }
                                    now2=now1;
                                    now1=now1->tail;
                                }
                                if(flag0==0){
                                    now2->tail=newinit;
                                }
                             }
                            if(t7->child->silbing==NULL)
                                 t7=NULL;
                            else{
                                t7 = t7->child->silbing->silbing;
                            }
                        }
                        t5 = t5->child->silbing;
                    }
                    type2->u.definition=type1->u.structures.structure;
                }
                else{
                    type1->u.structures.structure=NULL;
                    type2->u.definition=NULL;
                }
                FieldList newstruct = init_FieldList_(t3->sID,type2);
                HashNode newnode = init_HashNode_(newstruct,0);
                insert0_hashnode(newnode);
                return type1;
            }
        }
        else if(strcmp(t2->name,"LC")==0){
            type1->u.structures.name=NULL;
            Node*t4=t2->silbing;
            if(strcmp(t4->name,"DefList")==0){
                type1->u.structures.structure=NULL;
                Node*t5 = t4;//DefList → Def DefList | empty
                while(t5!=NULL){
                    Node*t6=t5->child->child;//Def → Specifier DecList SEMI 
                    Type typenew0=Specifier(t6);
                    Node*t7=t6->silbing;//DecList
                    while(t7!=NULL){//DecList → Dec | Dec COMMA DecList 
                        Node*t8 = t7->child->child;//t8代表VarDec     Dec → VarDec | VarDec ASSIGNOP Exp 
                        if(t8->silbing !=NULL){
                            printf("Error type 15 at Line %d:wrong initialization\n",t8->sline);
                        }
                        FieldList newinit = VarDec(t8,typenew0);
                        if(type1->u.structures.structure==NULL){
                            type1->u.structures.structure=newinit;
                        }
                        else{
                            FieldList now1=type1->u.structures.structure;
                            FieldList now2=type1->u.structures.structure;
                            int flag0=0;
                            while(now1!=NULL){
                                if(strcmp(now1->name,newinit->name)==0){
                                    flag0=1;
                                    printf("Error type 15 at Line %d: Redifned in Structure %s\n",t8->sline,newinit->name);
                                    break;
                                }
                                    now2=now1;
                                    now1=now1->tail;
                                }
                                if(flag0==0){
                                    now2->tail=newinit;
                                }
                             }
                            if(t7->child->silbing==NULL)
                                 t7=NULL;
                            else{
                                t7 = t7->child->silbing->silbing;
                            }
                        }
                    t5 = t5->child->silbing;
                    }
            }
            else{
                 type1->u.structures.structure=NULL;
            }
            return type1;
        }
        else if(strcmp(t2->name,"Tag")==0){
            Node*t3 = t2->child;//ID
            Type type3= searchstruct(t3->sID);
            if(type3==NULL){
                printf("Error type 17 at Line %d: Undifned  Structure %s\n",t3->sline,t3->sID);
                type1->u.structures.name=NULL;
                type1->u.structures.structure=NULL;
                return type1;
            }
            type1->u.structures.name=(char*)malloc(sizeof(char)*(strlen(t3->sID)+1));
            strcpy(type1->u.structures.name,t3->sID);
            type1->u.structures.structure=type3->u.definition;
            return type1;
        }
    }
}

FieldList VarDec(Node*root,Type type){// VarDec -> ID| VarDec LB INT RB
    Node* t1=root->child;
    if(strcmp(t1->name,"ID")==0){
        FieldList newfield = (FieldList)(malloc(sizeof(struct FieldList_)));
        newfield->tail=NULL;
        newfield->inter=NULL;
        newfield->flag1=0;
        newfield->type=type;
        newfield->name=t1->sID;
        return newfield;
    }
    else if(strcmp(t1->name,"VarDec")==0){
        FieldList temp1 = VarDec(t1,type);
        if(temp1->type->kind != ARRAY){
            Type typetemp = (Type)malloc(sizeof(struct Type_));
            typetemp->kind=ARRAY;
            typetemp->u.array.elem=temp1->type;
            typetemp->u.array.size=t1->silbing->silbing->snum;
            temp1->type=typetemp;
        }
        else{
             if(transflag==1){
                printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type. \n");
             }
             transflag=0;
             Type typetemp1 = (Type)malloc(sizeof(struct Type_));
             typetemp1->kind=ARRAY;
             Type new1 = temp1->type;
             Type new2=NULL;
             while(new1 != NULL && new1->kind == ARRAY){
                new1 = new1->u.array.elem;
                new2 = new1;
             }
             typetemp1->u.array.size=t1->silbing->silbing->snum;
             typetemp1->u.array.elem=new1;
             new2->u.array.elem=typetemp1;
        }
        return temp1;
    }
}