/*
//    COEN 383 Project 2
//    Group: 1
//    File: hpf.c
*/

#include "common.h"
#include <assert.h>

int last_q;

void Print_HPF_Stats(process* ptr,int* fs,int* ls,int* done) {
    float avg_response_time = 0,
	  avg_turn_around_time = 0,
	  throughput = 0,
	  avg_waiting_time = 0;
    int processes = 0;


    float avg_rt[4] = {0}, avg_tat[4] = {0}, avg_wt[4] = {0}, th[4] = {0};
    int proc_cnt[4] = {0};

    for(int i=0;i<4;i++) {
	avg_rt[i] = 0; avg_tat[i] = 0;
	avg_wt[i] = 0; th[i] = 0;
	proc_cnt[i] = 0;
    }

    for(int i=0;i<NUMBER_OF_PROCS;i++) if(done[ptr[i].pid]) {
	processes++;
	avg_response_time += fs[ptr[i].pid] - ptr[i].arrival_time + 1;
	avg_turn_around_time += ls[ptr[i].pid] - ptr[i].arrival_time + 1;
	assert((ls[ptr[i].pid] - ptr[i].arrival_time + 1) >= ptr[i].expected_runtime);
	avg_waiting_time += (ls[ptr[i].pid] - ptr[i].arrival_time + 1) - ptr[i].expected_runtime;

	//Priority specific statistics collection

	int ix = ptr[i].priority - 1; // Get the array index
	proc_cnt[ix]++;
	avg_rt[ix] += fs[ptr[i].pid] - ptr[i].arrival_time + 1;
	avg_tat[ix] += ls[ptr[i].pid] - ptr[i].arrival_time + 1;
	avg_wt[ix] += (ls[ptr[i].pid] - ptr[i].arrival_time + 1) - ptr[i].expected_runtime;
    }



    for(int i=0;i<4;i++) { //Priority specific statistics calculation
	avg_rt[i] /= proc_cnt[i];
	avg_tat[i] /= proc_cnt[i];
	avg_wt[i] /= proc_cnt[i];
	th[i] = ((1.0*proc_cnt[i]) / (1.0*last_q));
    }

    printf("\n***Overall statistics***\n");
    printf("Total completed processes: %d\n",processes);
    avg_response_time /= processes;
    avg_turn_around_time /= processes;
    avg_waiting_time /= processes;
    printf("Avg. Response time: %f\nAvg. Turn around-time: %f\nAvg. Waiting time: %f\n",avg_response_time,avg_turn_around_time,avg_waiting_time);
    throughput = (1.0*(processes))/(1.0*last_q);
    printf("Avg. throughput: %f\n",throughput);

    // Display priority specific stats

    printf("\n***Priority specific statistics***\n");

    for(int i=0;i<4;i++) {
	printf("Priority %d:\n",i+1);
	printf("Avg. Response time: %f\nAvg. Turn around-time: %f\nAvg. Waiting time: %f\n",avg_rt[i],avg_tat[i],avg_wt[i]);
	printf("Avg. throughput: %f\n",th[i]);
    }
}



int hpf_preemptive(process *ptr) {

    last_q = QUANTA;

    qsort(ptr, NUMBER_OF_PROCS, sizeof(process), compare_arrival_times);
    process Q[4][NUMBER_OF_PROCS];
    int ix[4] = {0,0,0,0}; //index of free position in the the priority queue
    int prev[4] = {-1,-1,-1,-1}; // index of the process which was last served

    int first_touch[NUMBER_OF_PROCS]; // Store information about when the process received CPU for the first time
    int last_touch[NUMBER_OF_PROCS]; // Store information about ^....................................^ last time
    int done[NUMBER_OF_PROCS];

    // Initialize arrays
    for(int i=0;i<NUMBER_OF_PROCS;i++) {
	first_touch[i] = -1;
	last_touch[i] = -1;
	done[i] = 0;
    }

    int x = 0; // pointer to the beginning of the process queue (ptr);
    for(int q=0;q<QUANTA + 50;q++) { // Allow some extra time for started processes to end beyond QUANTA

	if(q == QUANTA) { // Processes not serviced yet are to be nullified
	    for(int i=0;i<4;i++) for(int j=0;j<ix[i];j++) {
		if(first_touch[Q[i][j].pid] == -1) Q[i][j].expected_runtime = -1.0;
	    }
	}

	// BONUS-POINTS .. Handle aging, ie, bump the process to higher priority queue
	// after it spends 5 quanta in current queue.

	// Age processes present in all queues (except priority 1)
	for(int i=1;i<4;i++) for(int j=0;j<ix[i];j++) if(Q[i][j].expected_runtime > 0) {
	    Q[i][j].age++;
	}

	// First, look for any new processes starting at this quantum
	// and populate the queue accordingly

	if(q < QUANTA) { // Populate only if quantum < 100
	    while(x < NUMBER_OF_PROCS && ptr[x].arrival_time <= q) {
		int pr = ptr[x].priority - 1;
		Q[pr][ix[pr]] = ptr[x];
		ix[pr]++;
		x++;
	    }
	}

	for(int i=1;i<4;i++) {
	    for(int j=0;j<ix[i];j++) if(Q[i][j].expected_runtime > 0 && Q[i][j].age == 5) {
		Q[i-1][ix[i-1]] = Q[i][j]; // Insert process into higher priority queue
		Q[i-1][ix[i-1]].age = 0; // Fresh process in this new queue
		Q[i-1][ix[i-1]].priority--;
		printf("Process %d aged one priority up\n",Q[i-1][ix[i-1]].pid);

		ix[i-1]++;
		Q[i][j].expected_runtime = -1; // Entry in previous queue effectively nullified
	    }
	}

	// check process from which queue is to be alloted this quantum
	int target_queue = -1;
	for(int i=0;i<4;i++) {
	    for(int j=0;j<ix[i];j++) if(Q[i][j].expected_runtime > 0.0) { // if expected_runtime <= 0.0 then the process has already completed
		target_queue = i;
		break;
	    }
	    if(target_queue != -1) break;
	}
	if(target_queue == -1) {
	    if(q < QUANTA) {
		printf("No jobs at quantum %d\n",q);
		continue;
	    } else {
		last_q = q;
		break;
	    }
	}
	int target_process_ix;
	if(prev[target_queue] == -1) target_process_ix = 0;
	else {
	    assert(ix[target_queue] != 0);
	    int i = (prev[target_queue]+1) % ix[target_queue];
	    while(Q[target_queue][i].expected_runtime <= 0) {
		i = (i+1)%ix[target_queue];
	    }
	    target_process_ix = i;
	}

	process* target_process = &(Q[target_queue][target_process_ix]);

	if(first_touch[target_process->pid] == -1) first_touch[target_process->pid] = q;
	last_touch[target_process->pid] = q;
	if(target_process->expected_runtime < 1.0) {
	    printf("Q %d: %d(%f,PRIORITY: %d)   (CPU idle for %f quantum)\n",q,target_process->pid,target_process->expected_runtime,target_process->priority,(1.0-(target_process->expected_runtime)));
	} else {
	    printf("Q %d: %d(%f,PRIORITY: %d)\n",q,target_process->pid,target_process->expected_runtime,target_process->priority);
	}
	target_process->expected_runtime -= 1.0;
	target_process->age = 0; // Process just serviced, reset age.
	if(target_process->expected_runtime <= 0) {
	    done[target_process->pid] = 1;
	} 
	prev[target_queue] = target_process_ix;
    }
    Print_HPF_Stats(ptr,first_touch,last_touch,done);
    return 0;
}

int hpf_nonpreemptive(process *ptr) {

    last_q = QUANTA;

    qsort(ptr, NUMBER_OF_PROCS, sizeof(process), compare_arrival_times);
    process Q[4][NUMBER_OF_PROCS];

    int ix[4] = {0,0,0,0}; //index of free position in the the priority queue
    int first_touch[NUMBER_OF_PROCS]; // Store information about when the process received CPU for the first time
    int last_touch[NUMBER_OF_PROCS]; // Store information about ^....................................^ last time
    int done[NUMBER_OF_PROCS];

    // Initialize arrays
    for(int i=0;i<NUMBER_OF_PROCS;i++) {
	first_touch[i] = -1;
	last_touch[i] = -1;
	done[i] = 0;
    }

    process* target_process = NULL;

    int x = 0; // pointer to the beginning of the process queue (ptr);
    for(int q=0;q<QUANTA + 50;q++) {

	if(q == QUANTA) { // Processes not serviced yet are to be nullified
	    for(int i=0;i<4;i++) for(int j=0;j<ix[i];j++) {
		if(first_touch[Q[i][j].pid] == -1) Q[i][j].expected_runtime = -1.0;
	    }
	}

	// BONUS-POINTS .. Handle aging, ie, bump the process to higher priority queue
	// after it spends 5 quanta in current queue.


	// Age processes present in all queues (except priority 1)
	for(int i=1;i<4;i++) for(int j=0;j<ix[i];j++) if(Q[i][j].expected_runtime > 0) {
	    Q[i][j].age++;
	}

	// First, look for any new processes starting at this quantum
	// and populate the queue accordingly
	if(q < QUANTA) {
	    while(x < NUMBER_OF_PROCS && ptr[x].arrival_time <= q) {
		int pr = ptr[x].priority - 1;
		Q[pr][ix[pr]] = ptr[x];
		ix[pr]++;
		x++;
	    }
	}

	for(int i=1;i<4;i++) {
	    for(int j=0;j<ix[i];j++) if(Q[i][j].expected_runtime > 0 && Q[i][j].age == 5) {
		if(target_process && target_process->pid == Q[i][j].pid) continue; // No benefit for doing this for the ongoing process, it already owns the CPU.
		Q[i-1][ix[i-1]] = Q[i][j]; // Insert process into higher priority queue
		Q[i-1][ix[i-1]].age = 0; // Fresh process in this new queue
		Q[i-1][ix[i-1]].priority--;
		printf("Process %d aged one priority up\n",Q[i-1][ix[i-1]].pid);

		ix[i-1]++;
		Q[i][j].expected_runtime = -1; // Entry in previous queue effectively nullified
	    }
	}

	if(!target_process) {

	    if(q >= QUANTA) { // End of simulation; break
		last_q = q;
		break;
	    }

	    // check process from which queue is to be alloted this quantum
	    int target_queue = -1;
	    for(int i=0;i<4;i++) {
		for(int j=0;j<ix[i];j++) if(Q[i][j].expected_runtime > 0.0) { // if expected_runtime <= 0.0 then the process has already completed
		    target_queue = i;
		    break;
		}
		if(target_queue != -1) break;
	    }
	    if(target_queue == -1) {
		printf("No jobs at quantum %d\n",q);
		continue;
	    }
	    int target_process_ix = 0;
	    while(target_process_ix < ix[target_queue] && Q[target_queue][target_process_ix].expected_runtime <= 0) {
		target_process_ix++;
	    }
	    target_process = &(Q[target_queue][target_process_ix]);
	} 

	if(first_touch[target_process->pid] == -1) first_touch[target_process->pid] = q;
	last_touch[target_process->pid] = q;

	if(target_process->expected_runtime < 1.0) {
	    printf("Q %d: %d(%f,PRIORITY: %d)   (CPU idle for %f quantum)\n",q,target_process->pid,target_process->expected_runtime,target_process->priority,(1.0-(target_process->expected_runtime)));
	} else {
	    printf("Q %d: %d(%f,PRIORITY: %d)\n",q,target_process->pid,target_process->expected_runtime,target_process->priority);
	}

	target_process->expected_runtime -= 1.0;
	target_process->age = 0; // Process just serviced, reset age.
	if(target_process->expected_runtime <= 0) {
	    done[target_process->pid] = 1;
	    target_process = NULL;
	}
    }
    Print_HPF_Stats(ptr,first_touch,last_touch,done);
    return 0;
}

