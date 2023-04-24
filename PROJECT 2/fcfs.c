#include <stdio.h>
#include <stdlib.h>
#include "stat.h"
#include "utility.h"

average_stats first_come_first_serve_np(linked_list * process_list) 
{

	printf(" \n First Come First Serve: \n ");

	if(process_list->head == NULL) 
	{ 
		// If no process is scheduled then we print an error
		fprintf(stderr,"Couldn't find any process to schedule");
	}

	average_stats resultStat;
	
	queue *process_queue = (queue *)create_queue(); // Creating a queue for FIFO behavior (FCFS)
	node * process_pointer = process_list->head; // Starting from the head of the linked list

	process_stat * scheduled_process = NULL;

	linked_list *ll = create_linked_list(); // Creating a linked list which will be processed later to compute averages
	int timer = 0; // Timer to keep track of the time quanta
	while(timer<100 || scheduled_process!=NULL) 
	{
		if(process_pointer != NULL) 
		{ 
			// Process start
			process * new_process = (process *)(process_pointer->data);
			if(new_process->arrival_time <= timer) 
			{
				process_stat * ps = (process_stat *) malloc(sizeof(process_stat));
				ps->proc = new_process;
				ps->wait_time = 0;
				ps->turnaround_time = 0;
				ps->response_time = 0;

				ps->run_time = 0;
				ps->start_time = -1;
				ps->end_time = -1;

				enqueue(process_queue,ps); // Adding a process to the queue
				process_pointer = process_pointer->next; // Moving on to the next node of the linked list
			}
		}

		
		if(scheduled_process == NULL && process_queue->size > 0) 
		{
			scheduled_process = (process_stat *) dequeue(process_queue); // De-queueing the process which will be processed in the next iteration
		}

		if(scheduled_process != NULL) 
		{
			process * proc = scheduled_process->proc;

			printf("%c",proc->pid);

			if(scheduled_process->start_time == -1) 
			{
				scheduled_process->start_time = timer;
			}
			scheduled_process->run_time++;

			if(scheduled_process->run_time >= proc->run_time) 
			{
				scheduled_process->end_time = timer; // If runtime crosses required time update the end time
				add_node(ll,scheduled_process);
				scheduled_process = NULL;
			}
		} 
		else 
		{
			printf("_");
		}
		timer++;
	}
	printf("\n");
	
	printf("\nProcess Name\t| Arrival Time | Run Time | Response Time | Wait Time | Turn Around Time | Priority |\n");
	node * ptr = ll->head;
	float avg_response_time = 0;
	float avg_wait_time = 0;
	float avg_turnaround = 0;
	int process_count = 0;
	while(ptr!=NULL) 
	{
		process_stat *stat = (process_stat *)ptr->data;
		if(stat == NULL) printf("No Stat\n");
		process *proc = (process *)stat->proc;
		if(proc == NULL) printf("No Process\n");

		// Obtaining statistics for the current process run
		float arrival_time = proc->arrival_time;
		float run_time = proc->run_time;
		float response_time = stat->start_time - arrival_time;
		float turnaround = stat->end_time - proc->arrival_time + 1;
		float wait_time = turnaround - run_time;
		unsigned char priority = proc->priority;

		// Average computation 
		avg_response_time += response_time;
		avg_wait_time += wait_time;
		avg_turnaround += turnaround;
		process_count++;
		printf("%16c|%14.1f|%10.1f|%15.1f|%11.1f| %17.1f| %9u|\n",proc->pid,arrival_time, run_time, response_time, wait_time, turnaround, priority);
		ptr = ptr->next;
	}
	resultStat.avg_response_time = avg_response_time / process_count;
	resultStat.avg_wait_time = avg_wait_time / process_count;
	resultStat.avg_turnaround = avg_turnaround / process_count;
	resultStat.avg_throughput = process_count;
	printf("-----------------------------------------------------------------------------------------------------\n");
	printf("%16c|%14.1f|%10.1f|%15.1f|%11.1f| %17.1f\n",' ',0.0, 0.0, resultStat.avg_response_time, resultStat.avg_wait_time, resultStat.avg_turnaround);
	
	return resultStat;
}


process_stat * create_process_stat(process* proc) 
{
	process_stat * ps = (process_stat *) malloc(sizeof(process_stat));
	ps->proc = proc;
	ps->wait_time = 0;
	ps->turnaround_time = 0;
	ps->response_time = 0;

	ps->run_time = 0;
	ps->start_time = -1;
	ps->end_time = -1;
	return ps;
}
