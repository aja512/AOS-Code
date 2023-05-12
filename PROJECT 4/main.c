/*
// COEN 383 AOS Project 4
// File: main.c
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "policy.h"
#include "utility.h"


int main(int argc, char* argv[]) {

    int simulation_Time = 0;
    void (*functionPicked)(memoryPages *);
    
    int simulation_run = 0;
    page *pg;

    // Statistics[i][j]: j=0(fifo), j=1(lru), j=2(lfu), j=3(mfu), j=4(random), j=5(total)
    int hit_statistics[5][6] = { 0 };
    int miss_statistics[5][6] = { 0 };

    float fifo_avg_hit = 0.0, lru_avg_hit = 0.0, lfu_avg_hit = 0.0, mfu_avg_hit = 0.0, rand_avg_hit = 0.0;
    float fifo_avg_miss = 0.0, lru_avg_miss = 0.0, lfu_avg_miss = 0.0, mfu_avg_miss = 0.0, rand_avg_miss = 0.0;

    float final_avg_hit = 0.0, final_avg_miss = 0.0;

    int avg_swapped[5][5] = { 0 };
    float fifo_avg_swap = 0.0, lru_avg_swap = 0.0, lfu_avg_swap = 0.0, mfu_avg_swap = 0.0, rand_avg_swap = 0.0;

    // Run 5 times
    while (simulation_run < 5) {
        printf("****************************************************************SIMULATION %d****************************************************************\n\n", simulation_run);
        int counter = 0;

        // Run each algorithm once
        while (counter < 5) {
            printf("\n\n------------------------------------------WORKLOAD FOR ");

            if (counter == 0) {                     // 1st simulation: FIFO
                functionPicked = firstInFirstOut;
                printf("FIFO ");
            }
            else if (counter == 1) {                // 2nd simulation: LRU
                functionPicked = leastRecentlyUsed;
                printf("LRU ");
            }
            else if (counter == 2) {                // 3rd simulation: LFU
                functionPicked = leastFrequentlyUsed;
                printf("LFU ");
            }
            else if (counter == 3) {                // 4th simulation: MFU
                functionPicked = MostFrequentlyUsed;
                printf("MFU ");
            }
            else {                                  // 5th simulation: Random
                functionPicked = randomPick;
                printf("RANDOM ");
            }        

            printf("ALGORITHM------------------------------------------\n");

            int processSwapping = 0;

            // Create linked list for processes
            linked_list * processList = generate_processes(PROCESS_COUNT, counter);

            // Print all processes
            node *current = processList->head;
            while(current != NULL) {
                process *currentData = current->data;
                if (currentData->pid < 123) printf("[%c] \tarrival_time=%d : service_time=%d \t page_count=%d \t current_page=%d \n", (char)currentData->pid, currentData->arrivalTime, currentData->serviceTime, currentData->page_count, currentData->current_page);
                else printf("[%d] \tarrival_time=%d : service_time=%d \t page_count=%d \t current_page=%d \n", currentData->pid, currentData->arrivalTime, currentData->serviceTime, currentData->page_count, currentData->current_page);
                current = current->next;
            }
            printf("\n\n");

            // Create and initialize free page list with 100 pages
            memoryPages pageList;
            initializePageList(&pageList);

           int processIndex = 0; // index to the start of process queue
           for(simulation_Time = 0; simulation_Time < DURATION_COUNT; simulation_Time++) {
            processIndex = 0;
            node * ptr = processList->head;
            while(ptr!=NULL && processIndex < PROCESS_COUNT && ((process *)ptr->data)->arrivalTime <= simulation_Time) {
                if(((process *)ptr->data)->serviceTime > 0) {
                    //To check for atleast four pages
                    if (checkForFourFreePages(&pageList)) {
                        // if its present then bring it in the memory
                        page *p = findAFreePage(&pageList);
                        process *proc = (process *) ptr->data;
                        p->pid = proc->pid;
                        p->pageNumber = proc->current_page;
                        p->pageAllocatedTime = (float) simulation_Time;
                        p->frequency = 1;
                        p->lastUsedTime = simulation_Time;
                        if (p->pid < 123)
                            printf("Page::: %d for process id::: %c allocated at time::: %f\n", p->pageNumber, p->pid,
                                   p->pageAllocatedTime);
                         else
                            printf("Page::: %d for process id::: %d allocated at time ::: %f\n", p->pageNumber, p->pid,
                                    p->pageAllocatedTime);
                         processSwapping++;
                     } else break;
                 }
                 processIndex++;
                 ptr = ptr->next;
               }
                // For every 100 msec the process reference another memory location using the locality of reference algorithm
                for (int ms = 0; ms < 10; ms++) {
                    node * ptr = processList->head;
                    for (int j = 0; j < processIndex; j++) {
                        process *proc = (process *)ptr->data;
                        if (proc->serviceTime> 0) {
                            proc->current_page = getPageByLocalityRef(proc->current_page, proc->page_count); // To update current page no
                            if (checkPageExistsInMemory(&pageList, proc->pid, proc->current_page)) {  // to check if page already allocated for the process in memory
                                // Increment hit statistic depending on the algorithm (& total to get average)
                                hit_statistics[simulation_run][counter]++;
                                hit_statistics[simulation_run][5]++;

                                pg = getPage(&pageList, proc->pid, proc->current_page);
                                if (pg == NULL) {
                                    printf("potential bug, got NULL from on a page that exists");
                                    return -1;
                                }
                                pg->frequency++;
                                pg->lastUsedTime = simulation_Time;
                                continue;
                            }
                            else {  // If page is not allocated in memory
                                // Increment miss statistic depending on the algorithm (& total to get average)
                                miss_statistics[simulation_run][counter]++;
                                miss_statistics[simulation_run][5]++;

                                // Try to find a free page
                                page* targetPage = findAFreePage(&pageList);

                                if (targetPage != NULL) {   // Memory is not full, assign page attributes to free page
                                    targetPage->pid = proc->pid;
                                    targetPage->pageNumber = proc->current_page;
                                    targetPage->frequency = 1;
                                    targetPage->pageAllocatedTime = simulation_Time;
                                    targetPage->lastUsedTime = simulation_Time;
                                }
                                else {  // If memory if full
                                    // Evict a page using the replacement algorithm then set targetPage to the newly freed page
                                    functionPicked(&pageList);
                                    targetPage = findAFreePage(&pageList);

                                    // Assign page attributes
                                    targetPage->pid = proc->pid;
                                    targetPage->pageNumber = proc->current_page;
                                    targetPage->frequency = 1;
                                    targetPage->pageAllocatedTime = simulation_Time;
                                    targetPage->lastUsedTime = simulation_Time;;
                                }
                            }
                        }
                    ptr = ptr->next;
                   }
                }
               ptr = processList->head;
               for (int a = 0; a < processIndex; a++) {
                 process *proc = (process *)ptr->data;
                if (proc->serviceTime > 0) {
                    proc->serviceTime = proc->serviceTime - 1;
                    if (proc->serviceTime == 0) {
                        //process has finished execution . free pages in memory;
                        if (proc->pid < 123)  printf("Process %c done. Freeing memory!\n", proc->pid);
                        else printf("Process %d done. Freeing memory!\n", proc->pid);
                        freeAProcessMemory(&pageList, proc->pid);
                    }
                }
                ptr = ptr->next;
              }

            }
            avg_swapped[simulation_run][counter] = processSwapping;
            counter++;
        }
        printf("\n\n\n\n\n\n\n\n");
        simulation_run++;
    }

    /* Print the statistics over the 5 runs */

    for (int i=0;i<5;i++) {
        fifo_avg_hit += hit_statistics[i][0];
        fifo_avg_miss += miss_statistics[i][0];
        fifo_avg_swap += avg_swapped[i][0];

        lru_avg_hit += hit_statistics[i][1];
        lru_avg_miss += miss_statistics[i][1];
        lru_avg_swap += avg_swapped[i][1];

        lfu_avg_hit += hit_statistics[i][2];
        lfu_avg_miss += miss_statistics[i][2];
        lfu_avg_swap += avg_swapped[i][2];

        mfu_avg_hit += hit_statistics[i][3];
        mfu_avg_miss += miss_statistics[i][3];
        mfu_avg_swap += avg_swapped[i][3];

        rand_avg_hit += hit_statistics[i][4];
        rand_avg_miss += miss_statistics[i][4];
        rand_avg_swap += avg_swapped[i][4];

        final_avg_hit += hit_statistics[i][5];
        final_avg_miss += miss_statistics[i][5];
    }

    printf("\n\n ***************STATISTIC OUTPUTS***************\n\n");

    // FIFO
    printf("FIFO HIT/MISS RATIO: %0.1f hits / %0.1f misses, AVERAGE SUCCESSFUL SWAPS: %0.1f\n\n", fifo_avg_hit/5, fifo_avg_miss/5, fifo_avg_swap/5);

    // LRU
    printf("LRU HIT/MISS RATIO: %0.1f hits / %0.1f misses, AVERAGE SUCCESSFUL SWAPS: %0.1f\n\n", lru_avg_hit/5, lru_avg_miss/5, lru_avg_swap/5);

    // LFU
    printf("LFU HIT/MISS RATIO: %0.1f hits / %0.1f misses, AVERAGE SUCCESSFUL SWAPS: %0.1f\n\n", lfu_avg_hit/5, lfu_avg_miss/5, lfu_avg_swap/5);

    // MFU
    printf("MFU HIT/MISS RATIO: %0.1f hits / %0.1f misses, AVERAGE SUCCESSFUL SWAPS: %0.1f\n\n", mfu_avg_hit/5, mfu_avg_miss/5, mfu_avg_swap/5);

    // RANDOM
    printf("RANDOM PICK HIT/MISS RATIO: %0.1f hits / %0.1f misses, AVERAGE SUCCESSFUL SWAPS: %0.1f\n\n", rand_avg_hit/5, rand_avg_miss/5, rand_avg_swap/5);

    // Average hit/miss ratio
    printf("AVG HIT/MISS RATIO FOR ALL 5 RUNS: %0.1f hits/%0.1f misses\n\n", final_avg_hit/5, final_avg_miss/5);

}
