#include"semantic.h"

void Program_init(Node* root)
{
    init_hashtable();
    init_stacktable();
    if(root==NULL) return;
    ExtDefList(root->child);
}

void ExtDefList(Node* root)
{
    if(root==NULL) return;
    ExtDef(root->child);
    ExtDefList(root->child->silbing);

}

void ExtDef(Node* root)
{
    // if(root==NULL) return;
    // Specifier(root->child);
    // if(strcmp(root->child->silbing->name=="ExtDecList")==0)
    // {
    //     ExtDecList(root->child->silbing);
    // }
    // else if(strcmp(root->child->silbing->name=="FunDec")==0)
    // {
    //     FunDec(root->child->silbing);
    //     CompSt(root->child->silbing->silbing);
    // }

}