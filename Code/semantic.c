#include"semantic.h"

void Program_init(Node* root)
{
    init_hashtable();
    init_stacktable();
    push_stacktable();
    if(root==NULL) return;
    ExtDefList(root->child);
}

void ExtDefList(Node* root)
{
    if(root==NULL) return;
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
    else if(strcmp(t2,"FunDec")==0)
    {
       FunDec(t2,type1);
       CompSt(t3);
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
        else if(false==0){
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
    }
    else if(strcmp(t1->silbing->silbing->name,"VarList")==0){
        type1->u.function.num_of_parameter=0;
        type1->u.function.parameters=NULL;
        Node*t2=t1->silbing->silbing;
        Node*t3=t2->child;//VarList → ParamDec COMMA VarList | ParamDec 
        push_stacktable();
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
    }
}

FieldList ParamDec(Node*root){//ParamDec → Specifier VarDec 
     Node*t1 = root->child;
     Node*t2= t1-> silbing;
     Type type1 = Specifier(t1);
     if(type1 ==NULL)
       return NULL;
    FieldList newlist = VarDec(t2,type1);
    HashNode newnode=init_HashNode_(newlist,DEPTH);
    insert_hashnode(newnode);
    return newlist;
}

void Compst(Node* root){//由于函数形参也是函数的局部作用域，因此Compst中将不压栈，请调用时自己看情况是否压栈！但语句块分析完毕后退栈应当在Compst中进行
    
}

void DefList(Node*root){// DefList -> Def DefList| (empty)
     Node*t1=root->child;
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
       DecList(t1->silbing->silbing);
   }
}

void Dec(Node*root,Type type){//Dec → VarDec | VarDec ASSIGNOP Exp 
   Node*t1=root->child;
   FieldList newlist = VarDec(t1,type);
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
   if(strcmp(t1->name,"ID")==0){
    return serchvar(t1->sID);
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
   else if(strcmp(t1->name,"EXP")==0){
    if(strcmp(t2->name,"LB")==0){//Exp LB Exp RB 
        if(Exp(t3)==NULL || !(Exp(t3)->kind==BASIC && Exp(t3)->u.baisc==0)){
            printf("Error type 12 at Line %d: array operator error\n",t3->sline);
        }
        if(Exp(t1)==NULL || Exp(t1)->kind != ARRAY){
            printf("Error type 10 at Line %d: not array \n",t1->sline);
            return NULL;
        }
        return Exp(t1)->u.elem;
    }
    else if(strcmp(t2->name,"DOT")==0){//Exp DOT ID
       if(Exp(t1)==NULL || Exp(t1)->kind != STRUCTURE){
            printf("Error type 13 at Line %d: not structure \n",t1->sline);
            return NULL;
        }
        Type temp1=Exp(t1);
        Type res=serchstructval(temp1,t3->sID);
        if(res==NULL){
            printf("Error type 14 at Line %d: undefined structure val\n",t3->sline);
            return NULL
        }
        return res;
    }
    else if()
  }
}

Type Specifier(Node* root){
    //Specifier → TYPE | StructSpecifier
    Node*t1 = root -> child;
    Type type1 = (Type)malloc(sizeof(struct Type_));
    if(strcmp(t1->name,"TYPE")==0){
        type1->kind=BASIC;
        if(strcmp(t1->sID,"int")==0)
           type1->u.basic=0;
        else if(strcmp(t1->sID,"float")==0)
           type1->u.basic=1;
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
                return NULL
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
        FieldList newfield = (FieldList)(malloc(sizeof(FieldList_)));
        newfield->tail=NULL;
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