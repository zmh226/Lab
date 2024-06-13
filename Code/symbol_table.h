#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define HASH_SIZE 0x3fff
typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct HashNode_* HashNode;
HashNode hash_table[HASH_SIZE];
HashNode stack_table[100];
extern int DEPTH;


struct Type_
{
 enum { BASIC=1, ARRAY=2, STRUCTURE=3, FUNCTION=4 ,DEFINITION=5} kind;//5用来表示该节点是结构体的定义，3表示该节点是变量，类型是结构体
 union
 {
 // 基本类型
   int basic;//0表示int，1表示float
 // 数组类型信息包括元素类型与数组大小构成
   struct { Type elem; int size; } array;
 // 结构体类型信息是一个链表
   struct{ char *name; FieldList structure;} structures;//结构体的名字（如果没有即为NULL）以及结构体内部元素的链表
   FieldList definition;
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
     int flag1;//用来判断该变量是否为形参，默认为0，为形参则为1
     Type type; // 域的类型
     char* inter;//在中间代码中确认的名字
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

unsigned hash_pjw(char *name);

void insert_hashnode(HashNode node_);
void insert0_hashnode(HashNode node_);//将该哈希节点插入到最外层（结构体都是全局的）所以不会被删除，干脆直接插入到哈希表中即可
void push_stacktable();
void pop_stacktable();
void delete_node(HashNode deleted_node);
int typeEqual(Type t1,Type t2);//检查两者类型是否相同,1表示相同

int search_if_exist(char* name,Type type);
Type searchstruct(char* name);//寻找对应名字的结构体是否被定义，若没有则返回NULL
Type serchvar(char* name);//查找对应的变量的类型并返回对应类型，若该变量尚未定义，则返回NULL
Type serchfunc(char* name);//查找对应的func的类型并返回对应类型，若该变量尚未定义，则返回NULL
Type serchstructval(Type type,char* name);//查找结构体中是否定义了名字为name的域,若有，返回对应域的类型，若没有，返回NULL
int Determin(char* name,Type type);//判断该变量能不能被定义（当前层），或者判断函数或结构体能不能被定义，若能返回0，不能则返回对应的错误类型序号,该函数只考虑重复定义类型的错误



#endif