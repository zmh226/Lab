#include "symbol_table.h"

int DEPTH=-1;


void init_hashtable()
{
    for (int i = 0; i < HASH_SIZE; i++)
    {
        hash_table[i] = NULL;
    }
}

void init_stacktable()
{
    for (int i = 0; i < 100; i++)
    {
        stack_table[i] = NULL;
    }
}


unsigned hash_pjw(char *name)
{
    unsigned val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~0x3fff)
            val = (val ^ (i >> 12)) & 0x3fff;
    }
    return val;
}

HashNode init_HashNode_(FieldList data_, int depth_)
{
    HashNode newnode = (HashNode)malloc(sizeof(struct HashNode_));
    newnode->data = data_;
    newnode->depth = depth_;
    newnode->next_in_bucket = NULL;
    newnode->next_in_same_scope = NULL;
    return newnode;
}

FieldList init_FieldList_(char *name, Type type_)
{
    FieldList newlist = (FieldList)malloc(sizeof(struct FieldList_));
    newlist->inter = NULL;
    newlist->flag1 = 0;
    newlist->type = type_;
    newlist->tail = NULL;
    if(name!=NULL){
    newlist->name = (char*)malloc((strlen(name)+1)*sizeof(char));
    strcpy(newlist->name,name);
    }
    else 
    newlist->name=NULL;
    return newlist;
}

void insert_hashnode(HashNode node_)
{
    unsigned val = hash_pjw(node_->data->name);
    if (hash_table[val] == NULL)
        hash_table[val] = node_;
    else
    {
        node_->next_in_bucket = hash_table[val];
        hash_table[val] = node_;
    }//横插入哈希表

    //HashNode temp=stack_table[DEPTH];
    //while(temp->next_in_same_scope!=NULL)
    //{
     //   temp=temp->next_in_same_scope;
    //}
    //temp->next_in_same_scope=node_;//纵插入栈

}
void insert0_hashnode(HashNode node_){
    unsigned val = hash_pjw(node_->data->name);
    if (hash_table[val] == NULL)
        hash_table[val] = node_;
    else
    {
        node_->next_in_bucket = hash_table[val];
        hash_table[val] = node_;
    }//横插入哈希表
}

void push_stacktable()
{
    //printf("hello");
    DEPTH++;
    HashNode newnode = (HashNode)malloc(sizeof(struct HashNode_));
    newnode->data = NULL;
    newnode->depth = DEPTH;
    newnode->next_in_bucket = NULL;
    newnode->next_in_same_scope = NULL;
    stack_table[DEPTH]=newnode;
    //printf("pushDepth:%d\n",DEPTH);
}

void pop_stacktable()
{
    HashNode temp=stack_table[DEPTH];
    HashNode temp_pre;
    while (temp->next_in_same_scope!=NULL)
    {
        temp_pre=temp;
        temp=temp->next_in_same_scope;
        delete_node(temp);
    }
    DEPTH--;
    //printf("popDepth:%d\n",DEPTH);
}

void delete_node(HashNode deleted_node)
{
    unsigned val = hash_pjw(deleted_node->data->name);
    if(hash_table[val]==deleted_node)
    {
        hash_table[val]=deleted_node->next_in_bucket;
    }
    else
    {
    HashNode temp=hash_table[val];
    while(temp->next_in_bucket!=deleted_node)
    {
        temp=temp->next_in_bucket;
    }
    if(temp->next_in_bucket->next_in_bucket!=NULL) temp->next_in_bucket=temp->next_in_bucket->next_in_bucket;
    else  temp->next_in_bucket=NULL;
    }
    //如何处理参数？
}

int typeEqual(Type t1,Type t2){
    if(t1==NULL||t2==NULL||t1->kind != t2->kind)
       return 0;
    if(t1==t2)
       return 1;
    if(t1->kind == BASIC){
        if(t1->u.basic == t2->u.basic)
          return 1;
        else return 0;
    }
    else if(t1->kind == ARRAY){
        int tnum1=0,tnum2=0;
        Type temp1 = t1,temp2=t2;
        while(temp1!=NULL){
            tnum1+=1;
            temp1=temp1->u.array.elem;
            if(temp1->kind != ARRAY)
            break;
        }
        while(temp2!=NULL){
            tnum2+=1;
            temp2=temp2->u.array.elem;
            if(temp2->kind != ARRAY)
            break;
        }
        if(tnum1==tnum2 && typeEqual(temp1,temp2)==1)
            return 1;
        else return 0;
    }
    else if(t1->kind == STRUCTURE){
        if(t1->u.structures.name!=NULL && t2->u.structures.name!= NULL && strcmp(t1->u.structures.name,t2->u.structures.name)==0)
          return 1;
        else
          return 0;
    }
    else if(t1->kind == FUNCTION){
        if(t1->u.function.num_of_parameter != t2->u.function.num_of_parameter || typeEqual(t1->u.function.return_parameter,t2->u.function.return_parameter)==0)
             return 0;
        int num1=t1->u.function.num_of_parameter;
        FieldList tp1 = t1->u.function.parameters;
        FieldList tp2 = t2->u.function.parameters;
        for(int i =0;i<num1;i++){
            if(typeEqual(tp1->type,tp2->type)==0)
               return 0;
        }
        return 1;
    }
    return 2;
}
int search_if_exist(char* name,Type type)
{
    unsigned val = hash_pjw(name);
    if(type == NULL) return -1;
    if(hash_table[val] == NULL) return 0;//未找到同名
    else
    {
        HashNode temp=hash_table[val];
        while(temp!=NULL)
        {
            if(strcmp(temp->data->name,name)==0)
            {
                if(typeEqual(temp->data->type,type)==1){
                    if(temp->depth!=DEPTH)
                       return 1;//在外层找到同名同类型
                    else
                       return 2;//在内层找到同名同类型
                }
            }
            temp=temp->next_in_bucket;
        }
        return 0;
    }
}

int Determin(char* name,Type type){
    unsigned val = hash_pjw(name);
    if(type==NULL) return -1;
    if(hash_table[val] == NULL) return 0;
    else
    {
        HashNode temp=hash_table[val];
        while(temp!=NULL)
        {
            if(strcmp(temp->data->name,name)==0)
            {
                if(type->kind == DEFINITION)
                     return 16;
                else if(type->kind == FUNCTION)
                     return 4;
                else{
                    if(temp->data->type->kind == DEFINITION)
                        return 3;
                    else{
                        if(temp->depth == DEPTH){
                            return 3;
                        }
                    }
                }
            }
            temp=temp->next_in_bucket;
        }
        return 0;
        }
    }

    Type serchvar(char* name){
        //printf("find\n");
        unsigned val = hash_pjw(name);
        if(hash_table[val] == NULL) return NULL;
        else{
            HashNode temp=hash_table[val];
            while(temp!=NULL){
                if((strcmp(temp->data->name,name)==0) && temp->data->type->kind != DEFINITION && temp->data->type->kind != FUNCTION){
                    //printf("%s\n",temp->data->name);
                    return temp->data->type;
                }
                temp=temp->next_in_bucket;
            }
        }
        return NULL;
    }

    Type serchfunc(char* name){
        unsigned val = hash_pjw(name);
        if(hash_table[val] == NULL) return NULL;
        else{
            HashNode temp=hash_table[val];
            while(temp!=NULL){
                if((strcmp(temp->data->name,name)==0) && temp->data->type->kind == FUNCTION){
                    return temp->data->type;
                }
                temp=temp->next_in_bucket;
            }
        }
        return NULL;
    }

    Type searchstruct(char* name){
        unsigned val = hash_pjw(name);
        if(hash_table[val] == NULL) return NULL;
        else{
            HashNode temp=hash_table[val];
            while(temp!=NULL){
                if((strcmp(temp->data->name,name)==0) && temp->data->type->kind == DEFINITION ){
                    return temp->data->type;
                }
                temp=temp->next_in_bucket;
            }
        }
        return NULL;
    }


    Type serchstructval(Type type,char* name){
        if(type==NULL || type->kind != STRUCTURE)
           return NULL;
        FieldList temp = type->u.structures.structure;
        while(temp!=NULL){
            if(strcmp(temp->name,name)==0)
               return temp->type;
            temp=temp->tail;
        }
        return NULL;
    }
