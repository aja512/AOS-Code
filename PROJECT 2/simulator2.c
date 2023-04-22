/*

//    COEN 383 OS Project 2
//    Group 1
//    File: simulator2.c
*/

#include "common.h"

int simulator2(char* s, process *ptr, process* (*f)(process*, int, int, process*), int (*compar)(const void *, const void*))
{
    int i=0;
	int j=0;
    int done_procs = 0;
    float total_idle_time = 0;
    float current_idle_time = 0;
    float current_response_time = 0;
    float current_turnaround_time = 0;
    float current_waiting_time = 0;
    float total_response_time =0;
    float total_turnaround_time =0;
    float total_waiting_time = 0;

    process* c_proc = NULL;

    // Sort based on compar function passed
    qsort(ptr, NUMBER_OF_PROCS, sizeof(process), compar);

    printf("expected order:\n");
    print_procs(ptr);
    printf("%s Simulation starting...\n\n", s);
    for (i=0; i<QUANTA; i++)
    {
        // Sort based on compar function passed
        qsort(ptr, NUMBER_OF_PROCS, sizeof(process), compar);
        current_idle_time = 0;    // var to measure idle time in this quanta
        if (done_procs == NUMBER_OF_PROCS) break; // if all procs are finished we break this loop
        c_proc = f(ptr, i, 0, c_proc);

        // If c_proc is NULL there are no available procs to run so print QUANTA header with idle time and continue.
        if (c_proc == NULL){
            printf("| QUANTA %02d:  I D L E  |\n",i);
            total_idle_time += 1.0;
            continue;
        }

        // First time this proc gets the cpu, we get response time from this
        if (c_proc->expected_runtime == c_proc->remaining_runtime){
            current_response_time = i - c_proc->arrival_time;
            total_response_time += current_response_time;
//            printf("\t - this process [p%02d] just started and had response time: %3.2f\n", c_proc->pid, current_response_time);
        }

        // Now we found a process to run and we process by subtracting 1 from the remaining time
        c_proc->remaining_runtime -= 1;

        // Check to see if this process finished, if so, we record stats
        if (c_proc->remaining_runtime <= 0){
            done_procs++;
            current_idle_time = (-1) * c_proc->remaining_runtime;
            c_proc->remaining_runtime = 0;
            current_turnaround_time = (i+1) - c_proc->arrival_time;
            total_turnaround_time += current_turnaround_time;
            current_waiting_time = current_turnaround_time - c_proc->expected_runtime;
            total_waiting_time += current_waiting_time;
//            printf("\t - process [p%02d] is done. turn around: %3.2f waiting: %3.2f\n", c_proc->pid, current_turnaround_time, current_waiting_time);
        }

        // Print the process id and its remaining runtime
        printf("| QUANTA %03d: ", i);  // QUANTA header
        printf("[p%02d](%2.1f)|", c_proc->pid, c_proc->remaining_runtime);

        // If the cpu was idle in this quanta print that and add it to the total idle time
        if (current_idle_time){
            printf("<< CPU IDLE for %2.1f QUANTA |", current_idle_time);
            total_idle_time += current_idle_time;
        }
        printf("\n");
    }

    // Finish the rest of the QUANTA if all procs finished
    for (j=i; j< QUANTA; j++){
        printf("| QUANTA %02d: I D L E * |\n",j);
        total_idle_time += 1.0;
    }

    // Finish the procs that already started
    while ((c_proc = f(ptr, i, 1, c_proc)) != NULL){
        // Now we found a process to run and we process by subtracting 1 from the remaining time
        c_proc->remaining_runtime -= 1;

        // Check to see if this process finished, if so, we record the cpu idle time in this quanta
        if (c_proc->remaining_runtime <= 0){
            done_procs++;
            current_idle_time = (-1) * c_proc->remaining_runtime;
            c_proc->remaining_runtime = 0;
            current_turnaround_time = (i+1) - c_proc->arrival_time;
            total_turnaround_time += current_turnaround_time;
            current_waiting_time = current_turnaround_time - c_proc->expected_runtime;
            total_waiting_time += current_waiting_time;
//            printf("\t - process [p%02d] is done. turn around: %3.2f waiting: %3.2f\n", c_proc->pid, current_turnaround_time, current_waiting_time);
        }
        
        // Print the process id and its remaining runtime
        printf("| QUANTA %03d: ", i);  // QUANTA header
        printf("[p%02d](%2.1f)|", c_proc->pid, c_proc->remaining_runtime);
        // If the cpu was idle in this quanta print that and add it to the total idle time
        if (current_idle_time){
            printf("<< CPU IDLE for %2.1f QUANTA |", current_idle_time);
            total_idle_time += current_idle_time;
        }
        printf("\n");

        i++;
    }
    int total_quanta = i;

    printf("================= Summary ====================================\n");
    if (NUMBER_OF_PROCS - done_procs == 0){
        printf("All procs finished after %d QUANTA\n", total_quanta);
    }else{
        printf("%d processes left un-finished after %d QUANTA\n", NUMBER_OF_PROCS - done_procs, total_quanta);
        for (i=0; i< NUMBER_OF_PROCS; i++){
            if (ptr[i].remaining_runtime > 0){
                printf("pid:%d remaining time: %f\n", ptr[i].pid, ptr[i].remaining_runtime);
            }
        }
    }

    printf("Average turn around time in this run: %3.1f quanta\n", total_turnaround_time / done_procs);
    printf("Average response time in this run: %3.1f quanta\n", total_response_time / done_procs);
    printf("Average waiting time in this run: %3.1f quanta\n", total_waiting_time / done_procs);
    printf("Throughput: %3.3f\n", (float)done_procs/(float)total_quanta);
    printf("CPU was idle %3.1f Quanta \n", total_idle_time);
    printf("==============================================================\n\n");

    return 0;
}
