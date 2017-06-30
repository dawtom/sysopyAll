#include <string.h>

typedef struct ListNode{
    char * name;
    char * surname;
    char * birthdate;
    char * email;
    char * phone;
    char * address;

    struct ListNode * next;
    struct ListNode * prev;
} ListNode;

typedef struct List {
    ListNode * head;
} List;

typedef struct TreeNode
{
    char * name;
    char * surname;
    char * birthdate;
    char * email;
    char * phone;
    char * address;

    /*int left;
    int right;
    */struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

/*typedef  struct Tree {
    TreeNode * root;
} Tree;*/

//********************************************************************************************
//********************************************************************************************
//********************************************************************************************

struct List * create_list();
void remove_list(List * list);
void remove_node(ListNode *node);
void add_element_to_list(List * list, ListNode * node);
void delete_element_from_list(List * list, ListNode * node_to_delete);
ListNode * create_node();
void print_list(List *list);
//List* sort_list (List *list);
static ListNode * sortListByName(ListNode * node);
void List_sort(List * list);
void find_node(List * list, char * surname);
void print_node(ListNode * node);
void checkMemoryAllocation(void * v);


TreeNode ** create_tree();
void remove_tree_node(TreeNode *node);
void remove_tree(TreeNode ** tree);
void print_tree(TreeNode * root);
void print_tree_node(TreeNode * node);
void insert_to_tree(TreeNode ** tree, TreeNode * node);
TreeNode * create_tree_node();
struct TreeNode * deleteNode(struct TreeNode* root, TreeNode * key);
struct TreeNode * minValueNode(struct TreeNode * node);
struct TreeNode* search(struct TreeNode * root, char * surname);
struct TreeNode ** sort_tree_by_name(struct TreeNode ** root);