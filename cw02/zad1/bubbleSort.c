#include <stdio.h>
#include <stdlib.h>


void sort(void* (*next_record)(void), void (*swap_records)(int, int), void (*set_pointer_to_beginning)(void), int rec_size_in_bytes, int number_of_records) {
    if(number_of_records <= 1)
    {
        return;
    }

    int index_of_last_unsorted_element;
    int were_changes_made = 2;      // 2 means that no changes were made

    for(index_of_last_unsorted_element = number_of_records - 1; index_of_last_unsorted_element >= 1; --index_of_last_unsorted_element)
    {
        set_pointer_to_beginning();
        were_changes_made = 2;

        unsigned char * prev = (unsigned char *) next_record();
        unsigned char * curr = NULL;
        int i;

        for(i = 1; i <= index_of_last_unsorted_element; ++i)
        {
            curr = (unsigned char *) next_record();
            if(curr[0] < prev[0])
            {
                //printf("I am here");
                swap_records(i-1, i);
                were_changes_made = 1;      //1 means that there were changes made
                free(curr);

            } else {
                free(prev);
                prev = curr;
            }
        }
    }

}

void shuffle(void (*swap_records)(int, int), void (*set_pointer_to_beginning)(void), int rec_size_in_bytes, int number_of_records){
    if(number_of_records <= 1) {
        return;
    }
    set_pointer_to_beginning();

    int i,j;
    for (i = number_of_records - 1; i >= 1; --i) {
        j = i % (i+1);
        swap_records(i,j);
    }
    return;
}