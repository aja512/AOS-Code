/*
//    COEN 383 Project 2
//    Group: 1
//    File: rr.c
*/

#include "common.h"

//Get index of the last served process
static int get_index(process* ptr, int last_serviced)
{
    for (int i=0; i< NUMBER_OF_PROCS; i++){
        if (ptr[i].pid == last_serviced)
        {
            return i;
        }
        else{
            continue;
        }
    } 
    return 0;
}
/*
next_prc_rr: This function returns a pointer to a valid process that should be process next.

Inputs:
ptr - pointer to the array of procs
Q - the current quanta to compare against arrival time
c - pointer to the last proc that had the cpu
last_serviced - pid of the last serviced proc

*/


process * next_proc_rr(process* ptr, int Q, process* c, int last_serviced){
    int i =0;
    int index = 0;
    //Get index of last served process
    index = get_index(ptr, last_serviced);

    //Traverse left to select next process to serve
    for(i=index+1; i<NUMBER_OF_PROCS; i++){
        if(Q >= ptr[i].arrival_time && ptr[i].remaining_runtime > 0){
            return &ptr[i];
        }
    }

    //Traverse right to select next process to serve
    for (i=0; i< index; i++){
        if (Q >= ptr[i].arrival_time && ptr[i].remaining_runtime > 0)
        {
            return &ptr[i];
        }
    }

    //Return same element in case there is no other process to serve
    if (c != NULL && c->remaining_runtime > 0) {
         return c; 
    }
    return NULL;
}