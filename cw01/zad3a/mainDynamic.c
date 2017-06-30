#include <stdio.h>
#include "list.h"
#include <time.h>
#include <stdlib.h>
#include <sys/times.h>
#include <dlfcn.h>


static long int ticks_per_sec = CLOCKS_PER_SEC;
const x = 1000;

struct tms *run_time;
struct tms previous;

double prev_usr_milis;
double prev_sys_milis;


void verify_func(void * fun_ptr);

int main(){

    srand(time(NULL));

    prev_sys_milis = prev_usr_milis = 0.0d;

    void * list_lib_handle = dlopen("libsysopy.so", RTLD_NOW);

    if(list_lib_handle == NULL) {
        printf("\nERROR: Couldn't load library. Error message:\n%s\n", dlerror());
    }

    void (*checkMemoryAllocation)(void *)               = dlsym(list_lib_handle, "checkMemoryAllocation");
    verify_func(checkMemoryAllocation);

    List * (*create_list)(void)         = dlsym(list_lib_handle, "create_list");
    verify_func(create_list);

    void (*add_element_to_list)(List *, ListNode *)    = dlsym(list_lib_handle, "add_element_to_list");
    verify_func(add_element_to_list);

    void (*remove_list)(List *)               = dlsym(list_lib_handle, "remove_list");
    verify_func(remove_list);

    void (*List_sort)(List *)                 = dlsym(list_lib_handle, "List_sort");
    verify_func(List_sort);

    void (*find_node)(List *, char *) = dlsym(list_lib_handle, "find_node");
    verify_func(find_node);




//************************************************************************************************************
    printf("Program started. Clocks per second: %ld\n\n", ticks_per_sec);

    int r = 0;
    char * tmpName;
    tmpName = (char *) malloc(20 * sizeof(char));
    char * buffer[50];
    char * buffPointer;

    run_time = (struct tms *) malloc(sizeof(struct tms));
    print_time();
    for (int j = 0; j < 1000000; ++j) {
        TreeNode ** tree = create_tree();
        remove_tree(tree);
        //printf("%d", j);
    }
    printf("Created and destroyed tree one milion times\n");
    print_time();

    for (int j = 0; j < 1000000; ++j) {
        ListNode * list = create_list();
        remove_list(list);
        //printf("%d", j);
    }
    printf("Created and destroyed list one milion times\n");
    print_time();

    TreeNode ** tree = create_tree();
    TreeNode ** nodes;
    nodes = (TreeNode *)malloc(x * sizeof(TreeNode));
    printf("Beginning to insert to tree");
    for (int i = 0; i < x; ++i) {

        nodes[i] = create_tree_node();
        //printf(tmpName);
        tmpName = "Name";
        r = rand();
        snprintf(buffer, sizeof(buffer), "%d", r);
        char * nameString = (char *) malloc(1 + strlen(tmpName)+ strlen(buffer) );
        strcpy(nameString,tmpName);
        strcat(nameString, buffer);
        ((*nodes[i]).name) = nameString;


        tmpName = "Surname";
        r = rand();
        snprintf(buffer, sizeof(buffer), "%d", r);
        nameString = (char *) malloc(1 + strlen(tmpName)+ strlen(buffer) );
        strcpy(nameString,tmpName);
        strcat(nameString, buffer);
        ((*nodes[i]).surname) = nameString;


        tmpName = "Email";
        r = rand();
        snprintf(buffer, sizeof(buffer), "%d", r);
        nameString = (char *) malloc(1 + strlen(tmpName)+ strlen(buffer) );
        strcpy(nameString,tmpName);
        strcat(nameString, buffer);
        ((*nodes[i]).email) = nameString;


        tmpName = "Birthdate";
        r = rand();
        snprintf(buffer, sizeof(buffer), "%d", r);
        nameString = (char *) malloc(1 + strlen(tmpName)+ strlen(buffer) );
        strcpy(nameString,tmpName);
        strcat(nameString, buffer);
        ((*nodes[i]).birthdate) = nameString;

        tmpName = "Phone";
        r = rand();
        snprintf(buffer, sizeof(buffer), "%d", r);
        nameString = (char *) malloc(1 + strlen(tmpName)+ strlen(buffer) );
        strcpy(nameString,tmpName);
        strcat(nameString, buffer);
        ((*nodes[i]).phone) = nameString;


        tmpName = "Address";
        r = rand();
        snprintf(buffer, sizeof(buffer), "%d", r);
        nameString = (char *) malloc(1 + strlen(tmpName)+ strlen(buffer) );
        strcpy(nameString,tmpName);
        strcat(nameString, buffer);
        ((*nodes[i]).address) = nameString;

        //print_time();
        insert_to_tree(tree, nodes[i]);
        //print_time();

        //printf(str3);
    }

    print_time();
    printf("Inserted 1000 nodes to tree");
    print_time();

    print_time();
    for (int l = 0; l < 100; ++l) {
        tree = sort_tree_by_name(tree);
    }
    print_time();
    printf("Sorted tree 100 times");

    //****************************************************************************

    ListNode * list = create_list();
    nodes = (ListNode *)malloc(x * sizeof(ListNode));
    for (int i = 0; i < x; ++i) {

        nodes[i] = create_node();
        //printf(tmpName);
        tmpName = "Name";
        r = rand();
        snprintf(buffer, sizeof(buffer), "%d", r);
        char * nameString = (char *) malloc(1 + strlen(tmpName)+ strlen(buffer) );
        strcpy(nameString,tmpName);
        strcat(nameString, buffer);
        ((*nodes[i]).name) = nameString;


        tmpName = "Surname";
        r = rand();
        snprintf(buffer, sizeof(buffer), "%d", r);
        nameString = (char *) malloc(1 + strlen(tmpName)+ strlen(buffer) );
        strcpy(nameString,tmpName);
        strcat(nameString, buffer);
        ((*nodes[i]).surname) = nameString;


        tmpName = "Email";
        r = rand();
        snprintf(buffer, sizeof(buffer), "%d", r);
        nameString = (char *) malloc(1 + strlen(tmpName)+ strlen(buffer) );
        strcpy(nameString,tmpName);
        strcat(nameString, buffer);
        ((*nodes[i]).email) = nameString;


        tmpName = "Birthdate";
        r = rand();
        snprintf(buffer, sizeof(buffer), "%d", r);
        nameString = (char *) malloc(1 + strlen(tmpName)+ strlen(buffer) );
        strcpy(nameString,tmpName);
        strcat(nameString, buffer);
        ((*nodes[i]).birthdate) = nameString;

        tmpName = "Phone";
        r = rand();
        snprintf(buffer, sizeof(buffer), "%d", r);
        nameString = (char *) malloc(1 + strlen(tmpName)+ strlen(buffer) );
        strcpy(nameString,tmpName);
        strcat(nameString, buffer);
        ((*nodes[i]).phone) = nameString;


        tmpName = "Address";
        r = rand();
        snprintf(buffer, sizeof(buffer), "%d", r);
        nameString = (char *) malloc(1 + strlen(tmpName)+ strlen(buffer) );
        strcpy(nameString,tmpName);
        strcat(nameString, buffer);
        ((*nodes[i]).address) = nameString;

        //print_time();
        add_element_to_list(list, nodes[i]);
        //print_time();
        //print_node(nodes[i]);
        //printf(str3);
    }
    printf("Inserted 1000 nodes to list\n");

    print_time();
    delete_element_from_list(list, nodes[1]);
    printf("Deleted optimistic element from list\n");
    print_time();
    delete_element_from_list(list, nodes[999]);
    printf("Deleted pesimistic element from list\n");
    print_time();
    print_time();
    search((*tree), "Surname1558901923");
    print_time();

    for (int k = 0; k < 100; ++k) {
        List_sort(list);
    }
    printf("Sorted list 100 times\n");
    print_time();


}

void print_time()
{
    previous = *run_time;
    times(run_time);

    double total_usr = (double) run_time -> tms_utime;
    double total_sys = (double) run_time -> tms_stime;


    double total_usr_milis = (total_usr / ticks_per_sec) * 10000.0d;
    double total_sys_milis = (total_sys / ticks_per_sec) * 10000.0d;

    printf("\tTotal time:  \t\t%4.3lf (+%4.3lf) s\n", total_usr_milis + total_sys_milis, total_usr_milis + total_sys_milis - prev_usr_milis - prev_sys_milis);
    printf("\tUser time:   \t\t%4.3lf (+%4.3lf) s\n", total_usr_milis, total_usr_milis - prev_usr_milis);
    printf("\tSystem time: \t\t%4.3lf (+%4.3lf) s\n\n", total_sys_milis, total_sys_milis - prev_sys_milis);

    prev_usr_milis = total_usr_milis;
    prev_sys_milis = total_sys_milis;
}

void verify_func(void * fun_ptr)
{
    if(!fun_ptr)
    {
        printf("Error occured during loading symbol:\n%s", dlerror());
        exit(17);
    }
    return;
}