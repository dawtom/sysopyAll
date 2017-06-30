#ifndef _SYSOPY_BUBBLE_SORTER_H_
#define _SYSOPY_BUBBLE_SORTER_H_

void swap_records_lib(int a, int b);
void swap_records_sys(int a, int b);

void * sys_next_record();
void * lib_next_record();

void set_pointer_to_beginning_lib();
void set_pointer_to_beginning_sys();

#endif //_SYSOPY_BUBBLE_SORTER_H_
