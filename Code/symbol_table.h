#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

#include <stdio.h>
#define HASH_SIZE 0x3fff
typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct HashNode_* HashNode;
HashNode hash_table[HASH_SIZE];
HashNode stack_table[100];
extern int DEPTH;


struct Type_
{
 enum { BASIC=1, ARRAY=2, STRUCTURE=3, FUNCTION=4 } kind;
 union
 {
 // 基本类型
   int basic;
 // 数组类型信息包括元素类型与数组大小构成
   struct { Type elem; int size; } array;
 // 结构体类型信息是一个链表
   FieldList structure;
   struct 
   {
      Type return_parameter;
      int num_of_parameter;
      FieldList parameters;
   }function;
   
 } u;
 };

struct FieldList_
{
     char* name; // 域的名字
     Type type; // 域的类型
     FieldList tail; // 下一个域
};

struct HashNode_
{
    FieldList data;//域的信息
    int depth;//作用域深度
    HashNode next_in_bucket;// 下一个桶内节点
    HashNode next_in_same_scope;//下一个作用域内节点
};


void init_hashtable();
void init_stacktable();
HashNode init_HashNode_(FieldList data_,int depth_);
FieldList init_FieldList_(char* name,Type type_);


void insert_hashnode(HashNode node_);
void push_stacktable();
void pop_stacktable();
void delete_node(HashNode deleted_node);

int search_if_exist(char* name,Type type);



#endif