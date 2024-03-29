#include <stdio.h>
#include <stdlib.h>
#include "process.h"
#include "utility.h"
#include <time.h>

int compare(void * data1, void * data2) {
	process *p1 = (process *)data1;
	process *p2 = (process *)data2;
	if(p1->arrival_time < p2->arrival_time) {
		return -1;
	} else if(p1->arrival_time == p2->arrival_time){
		return 0;
	} else {
		return 1;
	}
}

linked_list * generate_processes(int n, int counter) {
	linked_list * process_list = create_linked_list();
	char pid = 'A';
	unsigned char priority;
	float arrival_time, run_time;
	time_t t;


   /* Intializes random number generator */
   srand((long long)time(NULL) + (long long)counter * 123456789);

	if(process_list == NULL) fprintf(stderr,"Unable to create Linked List\n");

	while(n--) {
		arrival_time = rand() % 100;
		run_time = (float)((rand() % 100) + 1)/10;
		priority = (rand()%4)+1;
		process *p = create_process(pid,arrival_time,run_time,priority);

		add_node(process_list,p);
		pid++;
	}

	sort(process_list,compare);

	node * ptr = process_list->head;
	pid = 'A';
	while(ptr!=NULL) {
		((process *)ptr->data)->pid = pid;
		if(pid == 'Z') pid = 'a' - 1;
		pid++;
		ptr=ptr->next;
	}

	return process_list;
}

process * create_process(char pid, float arrival_time, float run_time, unsigned char priority) {
	process * proc = (process *) malloc(sizeof(process));
	proc->pid = pid;
	proc->arrival_time = arrival_time;
	proc->run_time = run_time;
	proc->priority = priority;
	return proc;
}

preemptive_process * create_preemptive_process(process* proc) {
	preemptive_process * pp = (preemptive_process *) malloc(sizeof(preemptive_process));
	pp->pid = proc->pid;
    pp->arrival_time = proc->arrival_time;
	pp->run_time = proc->run_time;
	pp->priority = proc->priority; 
    pp->time_left = 0.0;
	pp->start_time = 0.0;
	pp->end_time = 0.0;
	return pp;
}

process * get_copy_of_process(process * proc){}
