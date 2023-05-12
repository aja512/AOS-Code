/*
// COEN 383 AOS Project 4
// File: policy.h
*/

#ifndef POLICY_H
#define POLICY_H

#include "utility.h"

// Implement relevant structures and declare all page replacement algorithms

#define DEBUG 1 // for MFU
#define PROCESS_COUNT 150
#define DURATION_COUNT 60
#define MAX_DURATION 6
#define TOTAL_PAGES_IN_MEMORY 100
#define EMPTY_PID -1

extern int pageSize[4];

typedef struct {
    int pid;
    int page_count;
    int arrivalTime;
    int serviceTime;
    int current_page;
} process;

typedef struct page {
    int pid;
    int pageNumber;
    struct page* nextPage;

    float pageAllocatedTime;
    float lastUsedTime;
    int frequency;
} page;

typedef struct {
    page* pages;
} memoryPages;

process *create_process(int pid, int page_count, int arrival_time, int service_time, int current_page);
linked_list *generate_processes(int n, int counter);

//void createPages(memoryPages* );
void initializePageList(memoryPages *);

void freeAProcessMemory(memoryPages* ,int pid);
page* findAFreePage(memoryPages* );
int processComparator(const void*,const void*);
// to check for atleast 4 free pages
int checkForFourFreePages(memoryPages*);
// get next page no by locality of reference
int getPageByLocalityRef(int current_page_no,int max_page_size);
//check if page exists in memory
int checkPageExistsInMemory(memoryPages* memoryPageList, int pid, int pageNumber);
page* getPage(memoryPages* memoryPageList, int pid, int pageNumber);
void displayPageList(memoryPages* memoryPageList);

void firstInFirstOut(memoryPages*);
void leastRecentlyUsed(memoryPages*);
void MostFrequentlyUsed(memoryPages*);
void leastFrequentlyUsed(memoryPages*);
void randomPick(memoryPages*);
#endif
