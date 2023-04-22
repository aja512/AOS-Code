#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMBER_OF_PROCS 30
#define RUNS_PER_ALGO 5
#define QUANTA 100

typedef struct {
    int pid;
    float arrival_time;
    float expected_runtime;
    float remaining_runtime;
    int priority;
    int age;
} process;

int compare_arrival_times(const void *, const void *);
int compare_expected_runtimes(const void *, const void *);
int compare_remaining_runtimes(const void *, const void *);
int compare_priorities(const void *, const void *);
int generate_procs(process *);
int print_procs(process *);

int simulator1(char*, process *, process* (*f)(process *, int, process *, int));
int simulator2(char*, process *, process* (*f)(process*, int, int, process*), int (*compar)(const void *, const void*));

process* next_proc_rr(process*, int, process*, int);
process* next_proc_fcfs(process*, int, process*, int);
process* next_proc_srt(process*, int, int, process*);
process* next_proc_sjf(process*, int, int, process*);

int hpf_preemptive(process*);
int hpf_nonpreemptive(process*);
void Print_HPF_Stats(process*,int*,int*,int*);


#endif