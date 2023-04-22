/*
//    COEN 383 Project 2
//    Group: 1
//    File: main.c
*/

#include "common.h"

// Main function, will parse the cmd line args and choose the right algo to run
int main(int argc, char *argv[])
{

    if(argc < 2){
        printf("scheduling : missing algorithmName operand\nTry './scheduling -h' for more information.\n");
        return 0;
    }

    if(argv[1] && strcmp(argv[1], "-h") == 0){
        printf("USAGE\n\t./scheduling algorithmName\n");
        printf("\nMandatory argument :\n");
        printf("algorithmName\n");
        printf("\tfcfs\n");
        printf("\t  First come first serve algorithm\n");
        printf("\trr\n");
        printf("\t  Round robin algorithm\n");
        printf("\tsjf\n");
        printf("\t  Shortest job first\n");
        printf("\tsrt\n");
        printf("\t  Shortest remaining time algorithm\n");
        printf("\thpfPre\n");
        printf("\t  Preemptive :Highest priority first algorithm\n");
        printf("\thpf\n");
        printf("\t  Non-Preemptive :Highest priority first algorithm\n");
        return 0;
    }


    process proc_list[NUMBER_OF_PROCS];
    int i;
    process *buff = malloc(NUMBER_OF_PROCS * sizeof(process));

    if(strcmp(argv[1], "fcfs") == 0){
        printf("Running fcfs %d times\n", RUNS_PER_ALGO);
        srand(0);
        for (i=0; i< RUNS_PER_ALGO; i++)
        {
            printf("run #%d:\n", i);
            generate_procs(buff);
            print_procs(buff);
//            fcfs(buff);
            simulator1("First Come First Serve", buff, next_proc_fcfs);
        }
    } else if(strcmp(argv[1], "rr") == 0) {
        printf("Running rr %d times\n", RUNS_PER_ALGO);
        srand(0);
        for (i=0; i< RUNS_PER_ALGO; i++)
        {
           printf("Running rr %d\n", i);
           generate_procs(buff);
           print_procs(buff);
//           rr(buff);
           simulator1("Round Robin", buff, next_proc_rr);
        }
    } else if(strcmp(argv[1], "sjf") == 0) {
        printf("Running sjf %d times\n", RUNS_PER_ALGO);
        srand(0);
        for (i=0; i< RUNS_PER_ALGO; i++)
        {
            printf("run #%d:\n", i);
            generate_procs(buff);
            print_procs(buff);
//            sjf(buff);
            simulator2("Shortest Job First", buff, next_proc_sjf, compare_expected_runtimes);
        }
    } else if(strcmp(argv[1], "srt") == 0) {
        printf("Running srt %d times\n", RUNS_PER_ALGO);
        srand(0);
        for (i=0; i< RUNS_PER_ALGO; i++)
        {
            printf("run #%d:\n", i);
            generate_procs(buff);
            print_procs(buff);
//            srt(buff);
            simulator2("Shortest Remaining Time", buff, next_proc_srt, compare_remaining_runtimes);
        }
    } else if(strcmp(argv[1], "hpf") == 0) {
        printf("Running hpf %d times\n", RUNS_PER_ALGO);
        srand(0);
        for (i=0; i< RUNS_PER_ALGO; i++)
        {
            printf("%d\n", i);
            generate_procs(buff);
            process a[NUMBER_OF_PROCS], b[NUMBER_OF_PROCS];
            for(int j=0;j<NUMBER_OF_PROCS;j++) {
                a[j] = buff[j];
                b[j] = buff[j];
            }

            hpf_nonpreemptive(b);
        }
    } else if(strcmp(argv[1], "hpfPre") == 0) {
        printf("Running hpf %d times\n", RUNS_PER_ALGO);
        srand(0);
        for (i=0; i< RUNS_PER_ALGO; i++)
        {
            printf("%d\n", i);
            generate_procs(buff);
            process a[NUMBER_OF_PROCS], b[NUMBER_OF_PROCS];
            for(int j=0;j<NUMBER_OF_PROCS;j++) {
                a[j] = buff[j];
                b[j] = buff[j];
            }

            hpf_preemptive(a);
        }
    } else {
        printf("Invalid options %s. See options with -h \n", argv[1]);
    }
    return 0;
}