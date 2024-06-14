#ifndef OBJECT_CODE
#define OBJECT_CODE

#include"semantic.h"
#include <string.h>

typedef struct Register_* Register;
typedef struct RegisterList_* RegisterList;
typedef struct Varible_* Varible;
typedef struct VarList_* VarList;
extern  char* reg_name[32];
//extern int OFFSET;

struct Register_
{
    char* var_name;
    int var_value;
    //char* name;
    bool isfree;
};
struct RegisterList_
{
    Register current;
    RegisterList outer;
};
struct Varible_
{
    //enum{temp,var,imm}if(strcmp()) vtype;
    char* vname;
    int offset;
    Varible next;
};

struct VarList_
{
    Varible head;
    Varible tail;
    int offset;
};

VarList varlist;
RegisterList registerlist[32];


void init_obj(FILE*file);
void init_registers();
void push_reg();
void pop_reg();
void init_varlist();
void insert_var(Varible v);
Varible search_var(char* name);
Varible new_var(char* name);
Varible new_arrayvar(char* name,int size);
void clear_varlist();
char* allocate(char* name);
char *find_reg(char *name);
char* get_reg(char* name);
void free_reg(char* name);
char* deal_value(char *name,FILE* file);
int get_offset(char* name);

void assign_v(char *name1, char *name2,FILE* file);
void assign_i(char *name1, int value,FILE* file);
void assign_from_const_reg(char* name1,char* const_reg_name,FILE *file);
void assign_addr(char * name1,char *name2,int opt,FILE* file);
void operation2(char*name1,char*name2,char*name3,char op,FILE *file);
void operation1(char* name1,char*name2,FILE* file);
void reallocate(char* register_name,FILE* file);


void enter_func(FILE * file);
void exit_func(FILE* file);
#endif