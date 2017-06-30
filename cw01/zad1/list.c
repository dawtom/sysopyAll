#include <stdio.h>
#include "list.h"
#include <time.h>
#include <stdlib.h>

//********************************************************************************************

List * create_list() {
  List * result = (List *) malloc(sizeof(List));
  checkMemoryAllocation(result);
  result -> head = NULL;
  return result;
}

//********************************************************************************************

void remove_node(ListNode *node)
{
    if(node == NULL)
    {
        printf("This node is NULL. Cannot remove.");
        return;
    }


    free(node);

    return;
}

//********************************************************************************************

void remove_list(List * list)
{
  if(list == NULL)
  {
    return;
  }

  ListNode * node = list -> head;

  while(node != NULL)
  {
    ListNode * next = node -> next;
    remove_node(node);
    node = next;
  }

  return;
}
//********************************************************************************************

void add_element_to_list(List * list, ListNode * node)
{
    if(list == NULL)
    {
        printf("Null list. Cannot add element.\n");
        return;
    }

    if(node == NULL)
    {
        printf("Null node. Cannot add element.\n");
        return;
    }

    ListNode * tmp = list -> head;
    if(list -> head != NULL)
    {
        while(tmp -> next != NULL)
        {
            tmp = tmp -> next;
        }
        tmp -> next = node;
        node -> prev = tmp;
        node -> next = NULL;

    } else {
        list -> head = node;
        node -> next = NULL;
        node -> prev = NULL;
    }
}

//********************************************************************************************

void delete_element_from_list(List * list, ListNode * node_to_delete)
{
    if(list == NULL) {
        printf("Null list. Cannot delete");
        return;
    }

    if(node_to_delete == NULL)
    {
        printf("Cannot delete null node");
        return;
    }

    ListNode * tmp = list -> head;

    while(tmp != NULL && tmp != node_to_delete)
    {
        tmp = tmp -> next;  //find the node
    }

    if(tmp == node_to_delete)
    {
        if(tmp -> prev == NULL)
        {
            list -> head = tmp -> next;
            if(list -> head != NULL)
            {
                list -> head -> prev = NULL;
            }
        } else {
            tmp -> prev -> next = tmp -> next;
            if(tmp -> next != NULL)
            {
                tmp -> next -> prev = tmp -> prev;
            }
        }
        remove_node(tmp);
    }

    return;
}

//********************************************************************************************

ListNode * create_node() {
    ListNode * node = (ListNode *) malloc(sizeof(ListNode));
    checkMemoryAllocation(node);

    node -> name = NULL;
    node -> surname = NULL;
    node -> birthdate = NULL;
    node -> email = NULL;
    node -> phone = NULL;
    node -> address = NULL;


    node -> next = NULL;
    node -> prev = NULL;

    return node;
}

//********************************************************************************************

void print_node(ListNode * node)
{
    printf("\t_______________________________________________________________\n");
    printf("\t| Name:            %s %s\n", node -> name, node -> surname);
    printf("\t| E-mail:  %s\n", node -> email);
    printf("\t| Phone:    %s \n", node -> phone);
    printf("\t| Address: \n", node -> address);
    printf("\t| Date of birth: ", node -> birthdate);
}

//********************************************************************************************

void print_list(List *list)
{
    if(list != NULL)
    {
        if(list -> head == NULL)
        {
            printf("Empty list\n");
            return;
        }
    } else {
        printf("Null list\n");
        return;
    }

    ListNode * tmp = list -> head;

    while(tmp != NULL)
    {
        print_node(tmp);
        tmp = tmp -> next;
    }
}

//********************************************************************************************


void List_sort(List * list) {
    list->head = sortListByName(list -> head);
}

static ListNode * sortListByName(ListNode * node) {

    if(node == NULL || node->next == NULL)
        return node; // zwroc posortowany node

    ListNode *curr, *smallest, *prev, *smallestPrev;
    curr = node;
    smallest = node;
    prev = node;
    smallestPrev = node;
    while(curr != NULL) {
        if(strcmp(curr->name,smallest->name) < 0 ) {
            smallestPrev = prev;
            smallest = curr;
        }
        prev = curr;
        curr = curr->next;
    }

    //zamien pierwszy z najmniejszym
    ListNode * tmp;
    if(smallest != node)
    {
        smallestPrev->next = node;
        tmp = node->next;
        node->next = smallest->next;
        smallest->next = tmp;
    }

    smallest->next = sortListByName(smallest->next);
    return smallest;
}

//********************************************************************************************

void find_node(List * list, char * surname)
{
    if(list == NULL) {
        printf("Null list. Cannot find anything");
        return;
    }

   /* if(node_to_find == NULL)
    {
        printf("Cannot find null node");
        return;
    }*/

    ListNode * tmp = list -> head;

    while(tmp != NULL && strcmp(tmp->surname,surname) != 0)
    {
        tmp = tmp -> next;  //find the node
    }

    if (tmp == NULL){
        printf("\nElement with surname \n");
        printf(surname);
        printf("\n has not been found\n");
    }
    else{
        print_node(tmp);
    }
}


//********************************************************************************************

void checkMemoryAllocation(void * v)
{
  if(v == NULL)
  {
    printf("Error while allocating memory. Exit with code 1\n");
    exit(1);
  }
}

//********************************************************************************************

TreeNode ** create_tree() {
    TreeNode ** result = (TreeNode **) malloc(sizeof(TreeNode));
    //result = NULL;
    checkMemoryAllocation(result);
    return result;
}

//********************************************************************************************

TreeNode * create_tree_node() {
    TreeNode * node = (TreeNode *) malloc(sizeof(TreeNode));
    checkMemoryAllocation(node);

    node -> name = NULL;
    node -> surname = NULL;
    node -> birthdate = NULL;
    node -> email = NULL;
    node -> phone = NULL;
    node -> address = NULL;

    node -> left = NULL;
    node -> right = NULL;

    return node;
}

//********************************************************************************************
void remove_tree_node(TreeNode *node)
{
    if(node == NULL)
    {
        printf("This node is NULL. Cannot remove.");
        return;
    }


    free(node);

    return;
}

//********************************************************************************************

void remove_tree(TreeNode ** root)
{
    /*if(root == NULL)
    {
        return;
    }

    TreeNode * node = root;

    while(node != NULL)
    {
        TreeNode * left = node -> left;
        TreeNode * right = node -> right;

        remove_node(node);
        remove_tree(left);
        remove_tree(right);
    }*/
    free(root);
    return;
}

//********************************************************************************************
void print_tree_node(TreeNode * node)
{
    if (node == NULL){
        printf("Null node");
    }
    else{
        printf("\t_______________________________________________________________\n");
        printf("\t| Name:            %s %s\n", node -> name, node -> surname);
        printf("\t| E-mail:  %s\n", node -> email);
        printf("\t| Phone:    %s \n", node -> phone);
        printf("\t| Address: \n", node -> address);
        printf("\t| Date of birth: ", node -> birthdate);
    }
}

//********************************************************************************************

/*void print_tree(TreeNode * root)
{
    if(root == NULL){
        printf("Empty tree\n");
        return;
    }

    if(root != NULL)
    {
        print_tree_node(root);
        print_tree(root->left);
        print_tree(root->right);
    }
}*/

void print_tree(TreeNode * root)
{
    if(root == NULL){
        printf("Empty tree\n");
        return;
    }

    if(root != NULL)
    {
        print_tree_node(root);
        printf("\nGoing left\n");printf(root->name);
        print_tree(root->left);
        printf("\nGoing right\n");printf(root->surname);
        print_tree(root->right);
    }
}

//********************************************************************************************

void insert_to_tree(TreeNode ** tree, TreeNode * node){

    TreeNode *temp = NULL;
    if (!(*tree)){
        temp = create_tree_node();
        temp->name = node->name;
        temp->surname = node->surname;
        temp->email = node->email;
        temp->address = node->address;
        temp->phone = node->phone;
        temp->birthdate = node ->birthdate;
        *tree = temp;
        return;
    }
    printf("\n");
    printf((*tree)->surname);printf("\n");
    printf(node->surname);printf("\n");
    //printf(strcmp((*tree)->surname,node->surname));printf("\n");
    if (strcmp((*tree)->surname,node->surname) > 0){
        printf("left\n");
        insert_to_tree(&(*tree) -> left, node);
    }
    else{
        printf("right\n");
        insert_to_tree(&(*tree) -> right, node);
    }


}

//********************************************************************************************

void insert_to_tree_by_name(TreeNode ** tree, TreeNode * node){

    TreeNode *temp = NULL;
    if (!(*tree)){
        temp = create_tree_node();
        temp->name = node->name;
        temp->surname = node->surname;
        temp->email = node->email;
        temp->address = node->address;
        temp->phone = node->phone;
        temp->birthdate = node ->birthdate;
        *tree = temp;
        return;
    }
    printf("\n");
    printf((*tree)->name);printf("\n");
    printf(node->name);printf("\n");
    //printf(strcmp((*tree)->surname,node->surname));printf("\n");
    if (strcmp((*tree)->name,node->name) > 0){
        printf("left\n");
        insert_to_tree(&(*tree) -> left, node);
    }
    else{
        printf("right\n");
        insert_to_tree(&(*tree) -> right, node);
    }


}

//********************************************************************************************

struct TreeNode * deleteNode(struct TreeNode* root, TreeNode * key)
{
    // base case
    if (root == NULL) return root;

    // If the key to be deleted is smaller than the root's key,
    // then it lies in left subtree
    if (strcmp(root->surname,key->surname) > 0)
        root->left = deleteNode(root->left, key);
        else
        {
            if (strcmp(root->surname,key->surname) < 0){
                root->right = deleteNode(root->right, key);
            }
            else
            {
                // node with only one child or no child
                if (root->left == NULL)
                {
                    struct TreeNode *temp = root->right;
                    free(root);
                    return temp;
                }
                else if (root->right == NULL)
                {
                    struct TreeNode *temp = root->left;
                    free(root);
                    return temp;
                }

                // node with two children: Get the inorder successor (smallest
                // in the right subtree)
                struct TreeNode * temp = minValueNode(root->right);

                // Copy the inorder successor's content to this node
                root->name = temp->name;
                root->surname = temp->surname;
                root->email = temp->email;
                root->address = temp->address;
                root->birthdate = temp->birthdate;
                root->phone = temp->phone;

                // Delete the inorder successor
                root->right = deleteNode(root->right, temp);

            }
        }

        // if key is same as root's key, then This is the node
        // to be deleted

    return root;
}

//********************************************************************************************

struct TreeNode * minValueNode(struct TreeNode * node)
{
    struct TreeNode* current = node;

    /* loop down to find the leftmost leaf */
    while (current->left != NULL)
        current = current->left;

    return current;
}

//********************************************************************************************

struct TreeNode* search(struct TreeNode * root, char * surname)
{
    if (root == NULL){
        printf("Element not found");
        return root;
    }
    //printf("I am here");

    printf(surname);printf("ff");printf(root->surname);printf("\n");
    if (strcmp(root->surname,surname) == 0){
        printf("Element found");
        return root;
    }else
    if (strcmp(root->surname,surname) < 0)
        return search(root->right, surname);
    else{
        return search(root->left, surname);
    }
}

//********************************************************************************************

struct TreeNode ** sort_tree_by_name(struct TreeNode ** root){
    TreeNode ** sortedTree = create_tree();
    TreeNode * tempNode = create_tree_node();

    while((*root) != NULL){
        tempNode = *root;
        insert_to_tree_by_name(sortedTree, tempNode);
        (*root) = deleteNode((*root),tempNode);
    }


    return sortedTree;
}

//********************************************************************************************
//********************************************************************************************
//********************************************************************************************
//********************************************************************************************
//********************************************************************************************