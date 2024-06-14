#include "object_code.h"

char*reg_name[32]={
    "$0",  "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2",
    "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5",
    "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
};
int OFFSET=0;
void init_obj(FILE *file)
{
    fprintf(file, ".data\n");
    fprintf(file, "_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(file, "_ret: .asciiz \"\\n\"\n");
    fprintf(file, ".globl main\n");

    fprintf(file, ".text\n");
    fprintf(file, "read:\n");
    fprintf(file, "  li $v0, 4\n");
    fprintf(file, "  la $a0, _prompt\n");
    fprintf(file, "  syscall\n");
    fprintf(file, "  li $v0, 5\n");
    fprintf(file, "  syscall\n");
    fprintf(file, "  jr $ra\n\n");

    fprintf(file, "write:\n");
    fprintf(file, "  li $v0, 1\n");
    fprintf(file, "  syscall\n");
    fprintf(file, "  li $v0, 4\n");
    fprintf(file, "  la $a0, _ret\n");
    fprintf(file, "  syscall\n");
    fprintf(file, "  move $v0, $0\n");
    fprintf(file, "  jr $ra\n");
}

void init_registers()
{
    for (int i = 0; i < 32; i++)
    {
        registerlist[i]=(RegisterList)malloc(sizeof(RegisterList));
        registerlist[i]->current = (Register)malloc(sizeof(Register));
        registerlist[i]->current->isfree = true;
        if (i == 0)
        {
            registerlist[i]->current->isfree = false;
            registerlist[i]->current->var_value = 0;
        }
        //registerlist[i]->current->name=(char*)malloc(sizeof((char*)(strlen(reg_name[i]+1))));
        //strcpy(registerlist[i]->current->name, reg_name[i]);
    }
}

void push_reg()
{
    for (int i = 1; i < 32; i++) // 0寄存器永远固定为0，所以push时无需多加一层
    {
        RegisterList newlist = (RegisterList)malloc(sizeof(RegisterList));
        newlist->current = (Register)malloc(sizeof(Register));
        newlist->current->isfree = true;
        //strcpy(newlist->current->name, reg_name[i]);
        newlist->outer = registerlist[i];
        registerlist[i] = newlist;
    }
}

void pop_reg()
{
    for (int i = 1; i < 32; i++) // 0寄存器只有一层，不需要pop
    {
        RegisterList temp = registerlist[i];
        registerlist[i] = temp->outer;
        free(temp);
    }
}

void init_varlist()
{
    varlist=(VarList)malloc(sizeof(VarList));
    varlist->head=(Varible)malloc(sizeof(Varible));
    varlist->tail=(Varible)malloc(sizeof(Varible));
    varlist->head = varlist->tail = NULL;
    varlist->offset = 8;
}

void insert_var(Varible v)
{
    if (varlist->head == NULL)
    {
        varlist->head = varlist->tail = v;
    }
    else
    {
        varlist->tail->next = v;
        varlist->tail = v;
    }
}

Varible search_var(char *name)
{
    if(varlist->head!=NULL)
    {
        if(strcmp(name,"t27")==0)
        {
            int a=0;
            ;
        }
    Varible temp = varlist->head;
    while (temp != NULL
    && strcmp(temp->vname, name) != 0)
    {
        temp = temp->next;
    }
    if (temp == NULL)
        return NULL;
    else if (strcmp(temp->vname, name) == 0)
        return temp;
    }
    else return NULL;
}

Varible new_var(char *name)
{
    Varible newvar = (Varible)malloc(sizeof(Varible));
    newvar->vname=(char*)malloc(sizeof((char*)(strlen(name)+1)));
    strcpy(newvar->vname, name);
    varlist->offset+=4;   
    newvar->offset=varlist->offset;
    newvar->next=NULL;
    insert_var(newvar);
    return newvar;
}

Varible new_arrayvar(char* name,int size)
{
    Varible newvar = (Varible)malloc(sizeof(Varible));
    newvar->vname=(char*)malloc(sizeof((char*)(strlen(name)+1)));
    strcpy(newvar->vname, name);
    varlist->offset+=size;   
    newvar->offset=varlist->offset;
    newvar->next=NULL;
    insert_var(newvar);
    return newvar;
}

void clear_varlist()
{
    if(varlist->offset>12)
    {
    Varible temp=varlist->head->next;
    free(varlist->head);
    while(temp!=varlist->tail)
    {
        Varible temp1=temp;
        temp=temp1->next;
        free(temp1);
    }
    free(temp);
    
    }
    else if(varlist->offset==12)
    {
        free(varlist->head);
    }
    init_varlist();
}

char *allocate(char *name)
{
    // if(v==NULL) return NULL;
    int i;
    for (i = 8; i <= 25; i++)
    {
        if (registerlist[i]->current->isfree == true)
        {
            registerlist[i]->current->isfree = false;
            // registerlist[i]->current->var_value=v->value;
            registerlist[i]->current->var_name=(char*)malloc(sizeof((char*)(strlen(name)+1)));
            strcpy(registerlist[i]->current->var_name, name);
            return reg_name[i];
        }
    }
    if (i == 26)
    {
        int tminnum = __INT_MAX__;
        int vminnum = __INT_MAX__;
        int tminreg = 0;
        int vminreg = 0;
        for (i = 8; i <= 25; i++)
        {
            if(registerlist[i]->current->isfree==false)
            {
            int num = atoi(registerlist[i]->current->var_name + 1);
            if (registerlist[i]->current->var_name[0] == 't')
            {
                if (num < tminnum)
                {
                    tminnum = num;
                    tminreg = i;
                }
            }
            if (registerlist[i]->current->var_name[0] == 'v')
            {
                if (num < vminnum)
                {
                    vminnum = num;
                    vminreg = i;
                }
            }
            }
        }
        if (tminreg != 0)
        {
            // registerlist[tminreg]->current->var_value=v->value;
            strcpy(registerlist[tminreg]->current->var_name, name);
            return reg_name[tminreg];
        }
        else if (tminreg == 0)
        {
            // registerlist[vminreg]->current->var_value=v->value;
            strcpy(registerlist[vminreg]->current->var_name, name);
            return reg_name[tminreg];
        }
    }
}

char *find_reg(char *name)
{
    for (int i = 8; i <= 25; i++)
    {
        if (registerlist[i]->current->isfree==false&&strcmp(name, registerlist[i]->current->var_name) == 0)
        {
            return reg_name[i];
        }
    }
    return NULL;
}

char *get_reg(char *name)
{
    if (find_reg(name) != NULL)
        return find_reg(name);
    else
        return allocate(name);
}
void free_reg(char* regname)
{
    //char* r1=find_reg(name);
    for(int i=8;i<=25;i++)
    {
        if(strcmp(regname,reg_name[i])) 
        {
            registerlist[i]->current->isfree=true;break;
        }
    }
}
char* deal_value(char *name,FILE* file) // 返回寄存器名或变量值（字符串形式)
{
    char* r1=get_reg(name);//给该变量分配一个新寄存器，可以直接用allocate,因为分配前一定不在寄存器中
    
    Varible var=search_var(name);
    if(var != NULL)
    {
        fprintf(file,"    lw %s,-%d($fp)\n",r1,var->offset);
    }//如果变量存在，则从栈中将其对应的值取出
    //var = new_var(name);
    //return get_reg(name);
    return r1;
   
}

int get_offset(char* name)
{
    Varible var=search_var(name);
    return var->offset;
}

void assign_v(char *name1, char *name2,FILE* file)
{
    // int value;
    // if (name2[0] == 't')
    // {
    //     for (int i = 8; i <= 25; i++)
    //     {
    //         if (strcmp(get_reg(name2), reg_name[i])==0)
    //         {
    //             value = registerlist[i]->current->var_value;
    //             break;
    //         }
    //     }
    // }
    // else if (name2[0] == 'v')
    // {
    //     Varible var2 = search_var(name2);
    //     value = var2->value;
    // }
    // if (name1[0] == 'v')
    // {
    //     Varible var1 = search_var(name1);
    //     var1->value = value;
    // }
    // for (int i = 8; i <= 25; i++)
    // {
    //     if (strcmp(get_reg(name1), reg_name[i])==0)
    //     {
    //         registerlist[i]->current->var_value = value;
    //         break;
    //     }
    // }
    char* r1=deal_value(name1,file);
    char* r2=deal_value(name2,file);
    fprintf(file,"    move %s,%s\n",r1,r2);
    Varible var1=search_var(name1);
    //Varible var2=search_var(name2);
    if(var1==NULL) {//如果变量不存在，栈中开一块新区域存该变量的值，并产生一个新的变量
        var1=new_var(name1);
        fprintf(file,"    subu $sp,$sp,4\n");    
    }
    fprintf(file,"    sw %s,-%d($fp)\n",r1,var1->offset);//无论name1对应的变量是否存在，在本次运算中值改变了，因此需要将其写回栈中
    //由于name2对应的变量值没变，且事先一定存在，所以不用写回
    free_reg(r1);
    free_reg(r2);//清理r1,r2寄存器的值，以便后续使用

}

void assign_i(char *name1, int value,FILE* file)
{
    // if (name1[0] == 'v')
    // {
    //     Varible var1 = search_var(name1);
    //     var1->value = value;
    // }
    // for (int i = 8; i <= 25; i++)
    // {
    //     if (strcmp(get_reg(name1), reg_name[i])==0)
    //     {
    //         registerlist[i]->current->var_value = value;
    //         break;
    //     }
    // }
    char* r1=deal_value(name1,file);
    fprintf(file,"    li %s,%d\n",r1,value);
    Varible var1=search_var(name1);
    if(var1==NULL) {//如果变量不存在，栈中开一块新区域存该变量的值，并产生一个新的变量
        var1=new_var(name1);
        fprintf(file,"    subu $sp,$sp,4\n");    
    }
    fprintf(file,"    sw %s,-%d($fp)\n",r1,var1->offset);//无论name1对应的变量是否存在，在本次运算中值改变了，因此需要将其写回栈中
    //由于name2对应的变量值没变，且事先一定存在，所以不用写回
    free_reg(r1);
}

void assign_from_const_reg(char* name1,char* const_reg_name,FILE *file)
{
    char* r1=deal_value(name1,file);
    fprintf(file,"    move %s,%s\n",r1,const_reg_name);
    Varible var1=search_var(name1);
    if(var1==NULL) {//如果变量不存在，栈中开一块新区域存该变量的值，并产生一个新的变量
        var1=new_var(name1);
        fprintf(file,"    subu $sp,$sp,4\n");    
    }
    fprintf(file,"    sw %s,-%d($fp)\n",r1,var1->offset);//无论name1对应的变量是否存在，在本次运算中值改变了，因此需要将其写回栈中
    //由于name2对应的变量值没变，且事先一定存在，所以不用写回
    free_reg(r1);
    //free_reg(r2);//清理r1,r2寄存器的值，以便后续使用


}

void operation2(char*name1,char*name2,char*name3,char op,FILE *file)
{
    // int value2,value3;
    // for (int i = 8; i <= 25; i++){
    //     if (strcmp(get_reg(name2), reg_name[i])==0)
    //     {
    //         value2 = registerlist[i]->current->var_value;
    //     }
    //     else if(strcmp(get_reg(name3),reg_name[i])==0)
    //     {
    //         value3=registerlist[i]->current->var_value;;
    //     }
    // }
    // if (name1[0] == 'v')
    // {
    //     Varible var1 = search_var(name1);
    //     switch(op)
    //     {
    //         case '+':{var1->value = value2+value3;break;}
    //         case '-':{var1->value = value2-value3;break;}
    //         case '*':{var1->value = value2*value3;break;}
    //         case '/':{var1->value = value2/value3;break;}
    //         default:break;
    //     }
        
    // }
    // for (int i = 8; i <= 25; i++)
    // {
    //     if (strcmp(get_reg(name1), reg_name[i])==0)
    //     {
    //         switch(op)
    //         {
    //             case '+':{registerlist[i]->current->var_value = value2+value3;break;}
    //             case '-':{registerlist[i]->current->var_value = value2-value3;break;}
    //             case '*':{registerlist[i]->current->var_value = value2*value3;break;}
    //             case '/':{registerlist[i]->current->var_value = value2/value3;break;}
    //         }
    //         break;
    //     }
    // }
    char* r1=deal_value(name1,file);
    char* r2=deal_value(name2,file);
    char* r3=deal_value(name3,file);
    switch(op)
    {
        case '+':{fprintf(file,"    add %s,%s,%s\n",r1,r2,r3);break;}
        case '-':{fprintf(file,"    sub %s,%s,%s\n",r1,r2,r3);break;}
        case '*':{fprintf(file,"    mul %s,%s,%s\n",r1,r2,r3);break;}
        case '/':{fprintf(file,"    div %s,%s\n",r2,r3);
                fprintf(file,"    mflo %s\n",r1);break;}
    }
    Varible var1=search_var(name1);
    if(var1==NULL) {//如果变量不存在，栈中开一块新区域存该变量的值，并产生一个新的变量
        var1=new_var(name1);
        fprintf(file,"    subu $sp,$sp,4\n");    
    }
    fprintf(file,"    sw %s,-%d($fp)\n",r1,var1->offset);//无论name1对应的变量是否存在，在本次运算中值改变了，因此需要将其写回栈中
    free_reg(r1);free_reg(r2);free_reg(r3);
}

void operation1(char* name1,char*name2,FILE* file)
{
    // int value;
    // if (name2[0] == 't')
    // {
    //     for (int i = 8; i <= 25; i++)
    //     {
    //         if (strcmp(get_reg(name2), reg_name[i])==0)
    //         {
    //             value = registerlist[i]->current->var_value;
    //             break;
    //         }
    //     }
    // }
    // else if (name2[0] == 'v')
    // {
    //     Varible var2 = search_var(name2);
    //     value = var2->value;
    // }
    // if (name1[0] == 'v')
    // {
    //     Varible var1 = search_var(name1);
    //     var1->value = -value;
    // }
    // for (int i = 8; i <= 25; i++)
    // {
    //     if (strcmp(get_reg(name1), reg_name[i])==0)
    //     {
    //         registerlist[i]->current->var_value = -value;
    //         break;
    //     }
    // }
    char* r1=deal_value(name1,file);
    char* r2=deal_value(name2,file);
    fprintf(file,"    sub %s,$0,%s\n",r1,r2);
    Varible var1=search_var(name1);
    //Varible var2=search_var(name2);
    if(var1==NULL) {//如果变量不存在，栈中开一块新区域存该变量的值，并产生一个新的变量
        var1=new_var(name1);
        fprintf(file,"    subu $sp,$sp,4\n");    
    }
    fprintf(file,"    sw %s,-%d($fp)\n",r1,var1->offset);//无论name1对应的变量是否存在，在本次运算中值改变了，因此需要将其写回栈中
    //由于name2对应的变量值没变，且事先一定存在，所以不用写回
    free_reg(r1);
    free_reg(r2);//清理r1,r2寄存器的值，以便后续使用
}

void assign_addr(char * name1,char *name2,int opt,FILE* file)
{
    char* r1=deal_value(name1,file);
    char* r2=deal_value(name2,file);
    if(opt==1)
    {
        fprintf(file,"    sw %s,0(%s)\n",r1,r2);
    }
    else if(opt==2)
    {
        fprintf(file,"    lw %s,0(%s)\n",r1,r2);
    }
    Varible var1=search_var(name1);
    //Varible var2=search_var(name2);
    if(var1==NULL) {//如果变量不存在，栈中开一块新区域存该变量的值，并产生一个新的变量
        var1=new_var(name1);
        fprintf(file,"    subu $sp,$sp,4\n");    
    }
    fprintf(file,"    sw %s,-%d($fp)\n",r1,var1->offset);//无论name1对应的变量是否存在，在本次运算中值改变了，因此需要将其写回栈中
    //由于name2对应的变量值没变，且事先一定存在，所以不用写回
    free_reg(r1);
    free_reg(r2);//清理r1,r2寄存器的值，以便后续使用


}

void reallocate(char* register_name,FILE* file)
{
    int i;
    
    for(i=0;i<32;i++)
    {
        if(strcmp(register_name,reg_name[i])==0)
        {
            if(registerlist[i]->current->isfree==false)
            {
            char *newreg_name=allocate(registerlist[i]->current->var_name);
            //registerlist[i]->current->isfree=true;
            strcpy(registerlist[i]->current->var_name," ");
            fprintf(file,"    move %s,%s\n",newreg_name,register_name);
            break;
            }
        }
    }
    
}

void enter_func(FILE * file)//被调用者进入，在紧接着“func"标签之处使用
{
    fprintf(file,"    subu $sp,$sp,8\n");
    // char* v1=deal_value("off",file);
    // fprintf(file,"    li %s,%d\n",v1,OFFSET);
    // OFFSET=12;
    // fprintf(file,"    sw %s,8($sp)\n",v1);
    // free_reg(v1);
    fprintf(file,"    sw $ra,4($sp)\n");
    fprintf(file,"    sw $fp,0($sp)\n");
    fprintf(file,"    addi $fp,$sp,8\n");

}

void exit_func(FILE* file)//被调用者退出，在识别到RETURN的时候使用
{
    // char* v1=deal_value("off",file);
    // fprintf(file,"    lw %s,-4($fp)\n",v1);
    fprintf(file,"    lw $ra,-4($fp)\n");
    fprintf(file,"    lw $fp,-8($fp)\n");
    fprintf(file,"    addi $sp,$sp,8\n");
    //fprintf(file,"    jr $ra\n");
}
