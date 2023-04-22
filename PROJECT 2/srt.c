/*
//    COEN 383 Project 2
//    Group: 1
//    File: srt.c
*/


#include "common.h"

/*
// next_prc_srt:
// This function returns a pointer to a valid process that should be process next.
//
//  Inputs:
//  ptr - pointer to the array of procs
//  Q - the current quanta to compare against arrival time
//  S - flag to ask for processes that have already started only
//  c - pointer to the last proc that had the cpu
*/

process * next_proc_srt(process* ptr, int Q, int S, process* nop){
    int i =0;
    qsort(ptr, NUMBER_OF_PROCS, sizeof(process), compare_remaining_runtimes);
    for (i=0; i< NUMBER_OF_PROCS; i++){
        if(S){
            if (Q >= ptr[i].arrival_time && ptr[i].remaining_runtime > 0 && ptr[i].remaining_runtime != ptr[i].expected_runtime) return &ptr[i];
        }else{
            if (Q >= ptr[i].arrival_time && ptr[i].remaining_runtime > 0) return &ptr[i];
        }
    }
    return NULL;
}