#include "symbol_table.h"

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
}

FieldList init_FieldList_(char *name, Type type_)
{
    FieldList newlist = (FieldList)malloc(sizeof(struct FieldList_));
    newlist->name = name;
    newlist->type = type_;
    newlist->tail = NULL;
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

    HashNode temp=stack_table[DEPTH];
    while(temp->next_in_same_scope!=NULL)
    {
        temp=temp->next_in_same_scope;
    }
    temp->next_in_same_scope=node_;//纵插入栈

}

void push_stacktable()
{
    HashNode newnode = (HashNode)malloc(sizeof(struct HashNode_));
    newnode->data = NULL;
    newnode->depth = DEPTH;
    newnode->next_in_bucket = NULL;
    newnode->next_in_same_scope = NULL;
    stack_table[DEPTH]=newnode;
    DEPTH++;
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

int search_if_exist(char* name,Type type)
{
    unsigned val = hash_pjw(name);
    if(hash_table[val] == NULL) return 0;//未找到同名变量
    else
    {
        HashNode temp=hash_table[val];
        while(temp->next_in_bucket!=NULL)
        {
            if(temp->data->name==name)
            {
                if(temp->data->type!=type)
                {
                   return 1;//值相同，类型不同
                }
                else 
                {
                    if(temp->depth==DEPTH) return 2;//单值重复定义
                    else return 3;//外层作用域定义同类型值
                }
            }
            temp=temp->next_in_bucket;
        }
        return 0;
    }
}