#include <stdio.h>
#include <stdlib.h>
#include "stat.h"
#include "utility.h"
#include <errno.h>

process_stat * create_process_stat(process* proc);
// process_stat: Output statistics, e.g. wait_time, turnaround_time, response_time, start_time, end_time, run_time;

int shortestJobsFirst(void * data1, void * data2) {
	// Compare service time of both processes
	int flag = (((process *)((process_stat *)data1)->proc)->run_time < ((process *)((process_stat *)data2)->proc)->run_time) ? 0 : 1;
	return flag;	// 0 if no sorting needed, 1 if processes must be switched
}

average_stats SJF_scheduler(linked_list *processes) {
	int quanta = 0;

	node *current = processes->head;
	if(current == NULL) perror("No processes in queue");	// If no processes are passed, throw error
	process_stat *next_process = NULL;

	// Generate scheduling queue for processes and linked list for final order
	queue *scheduler_q = (queue *)create_queue();
	linked_list *SJF_order = create_linked_list();

	printf("\nShortest Job First (Non-Preemptive):\n");
	while(quanta<100 || next_process!=NULL) {	// Iterate through 100 quanta, continue past only to finish last process
		if(current != NULL) {
			// Retreive all processes that arrive before current process has finished and before/during current quanta
			while(current!=NULL && ((process *)(current->data))->arrival_time <= quanta) {	
				process *data = (process *)(current->data);		// Retrieve data of current process
				enqueue(scheduler_q,create_process_stat(data));	// Add current process to beginning of scheduler queue
				sort(scheduler_q,shortestJobsFirst);			// SJF -> sort currently scheduled processes by service_time
				current = current->next;
			}
		}
		// If process not yet scheduled after current, take first one from SJF scheduling queue
		if(next_process == NULL && scheduler_q->size > 0) next_process = (process_stat *)(dequeue(scheduler_q));

		// Handle process that will run after current one
		if(next_process != NULL) {
			process *np = next_process->proc;
			printf("%c", np->pid);		// Add to timechart of processes per quanta

			// Set process start time to the current quanta
			if(next_process->start_time == -1) next_process->start_time = quanta;
			next_process->run_time++;

			if(next_process->run_time >= np->run_time) {
				next_process->end_time = quanta;
				add_node(SJF_order,next_process);
				next_process = NULL;	// Signify that process has finished
			}
		} 
		else  printf("_");				// No process scheduled next, add nothing to timechart
		quanta = quanta + 1;			// Continue to next quanta
	}
	printf("\n");	// Add linebreak after timechart

	// Print statistics for SJF scheduled process list
	return print_policy_stat(SJF_order);
}
