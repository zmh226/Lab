#ifndef NODE
#define NODE

typedef struct TreeNode{
    int sline;/* 所在行数 */
    int stype;/* 0表示语法单元,1表示一般词法单元,2表示ID,3表示type,4表示int,5表示float */
    char* name;/* 语法单元和词法单元的名字 */
    char* sID;/* ID的词素，类型的具体名称（int，float） */
    int snum;
    double snum1;
    struct TreeNode* child;/*最左边的孩子节点 */
    struct TreeNode* silbing;/*右边紧挨着的兄弟节点 */
 }Node;
 
#endif
