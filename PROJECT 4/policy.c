/*
// COEN 383 AOS Project 4
// File: policy.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "policy.h"


//Chk: process or page size
int pageSize[] = {5, 11, 17, 31};   // Process has random size of 5, 11, 17, or 31 MB

int compare(void * data1, void * data2) {
	process *p1 = (process *)data1;
	process *p2 = (process *)data2;
	if(p1->arrivalTime < p2->arrivalTime) {
		return -1;
	} else if(p1->arrivalTime == p2->arrivalTime){
		return 0;
	} else {
		return 1;
	}
}

process *create_process(int pid, int page_count, int arrival_time, int service_time, int current_page) {
    process *proc = (process *) malloc(sizeof(process));
    proc->pid = pid;
    proc->page_count = page_count;
    proc->arrivalTime = arrival_time;
    proc->serviceTime = service_time;
    proc->current_page = current_page;
    return proc;
}

linked_list *generate_processes(int n, int counter) {
    linked_list *process_list = create_linked_list();
    int pid = 65;       // Process ID: 65-90 (A-Z), 97-122 (a-z), 123+ (numbers)
    //chk
    int page_count;
    int arrival_time;
    int service_time;
    //chk
    int current_page = 0;
    time_t t;

    /* Intializes random number generator */
   srand((long long)time(NULL) + (long long)counter * 123456789);

	if(process_list == NULL) fprintf(stderr,"Unable to create Linked List\n");

	while(n--) {
        int index = rand() % 4;
        page_count = pageSize[index];

		arrival_time = rand() % DURATION_COUNT;         // Arrives within one minute (int from 0-59)
		service_time = (rand() % 5) + 1;                // Service time is 1-5 seconds
        //chk
		process *p = create_process(pid, page_count, arrival_time, service_time, current_page);

		add_node(process_list,p);
		pid++;
	}

	sort(process_list,compare);

	node * ptr = process_list->head;
	pid = 65;
	while(ptr!=NULL) {
		((process *)ptr->data)->pid = pid;
		if(pid == 90) pid = 96;
		pid++;
		ptr=ptr->next;
	}

	return process_list;
}

/*void createPages(memoryPages* listOfPages){
    listOfPages->pages = malloc(sizeof(page));
    page* it = listOfPages->pages;
    int i;
    for(i=0;i<TOTAL_PAGES_IN_MEMORY;i++) {
        it->pid = EMPTY_PID;
        it->pageNumber = i;
        it->nextPage = NULL;
        if(i < 99) {
            it->nextPage = malloc(sizeof(page));
            it = it->nextPage;
        }
    }
}*/

void initializePageList(memoryPages *pageList) {
    pageList->pages = malloc(sizeof(page));
    page *current = pageList->pages;
    for (int i = 0; i < TOTAL_PAGES_IN_MEMORY; i++) {
        current->pid = -1;
        current->pageNumber = -1;
        if(i < TOTAL_PAGES_IN_MEMORY-1) {
            current->nextPage = malloc(sizeof(page));
            current = current->nextPage;
        }
        else current->nextPage = NULL;
    }
}

void freeAProcessMemory(memoryPages* listOfPages,int pid) {
    page* it = listOfPages->pages;
    while(it) {
        if(it->pid == pid) {
            it->pid = -1;
            it->pageNumber = -1;
        }
        it = it->nextPage;
    }
}

page* findAFreePage(memoryPages* listOfPages) {
    page* it = listOfPages->pages;
    while(it) {
        if(it->pid == -1) {
            return it;
        }
        it = it->nextPage;
    }
    return NULL;
}

int processComparator(const void* process1,const void* process2) {
    return ((process*)process1)->arrivalTime - ((process*)process2)->arrivalTime;
}

// to check for atleast 4 free pages
int checkForFourFreePages(memoryPages* memoryPageList) {
    int pageCheckCount = 4;
    page* it = memoryPageList->pages;
    while(it) {
        if(it->pid == -1) { // page is free
            pageCheckCount--;
        }
        if(!pageCheckCount) return 1;
        it = it->nextPage;
    }
    return 0;
}

// get next page no by locality of reference
int getPageByLocalityRef(int current_page_no,int max_page_size){
    int r = rand()%10;
    int nexti;
    if(r < 7) {
        nexti = current_page_no+(rand()%3)-1 < 0;
    } else {
        nexti = rand()%max_page_size;
        while(abs(nexti-current_page_no) <= 1) {
            nexti = rand()%max_page_size;
        }
    }
    if(nexti < 0) {
        nexti = 0;
    }
    if(nexti >= 100){
        nexti = max_page_size-1;
    }
    return nexti;
}


//check if page exists in memory
int checkPageExistsInMemory(memoryPages* memoryPageList, int pid, int pageNumber) {
    page* it = memoryPageList->pages;
    while(it) {
        if(it->pid == pid && it->pageNumber == pageNumber) return 1;
        it = it->nextPage;
    }
    return 0;
}


page* getPage(memoryPages* memoryPageList, int pid, int pageNumber) {
    page* it = memoryPageList->pages;
    while(it) {
        if(it->pid == pid && it->pageNumber == pageNumber) return it;
        it = it->nextPage;
    }
    return NULL;
}

void displayPageList(memoryPages* memoryPageList) {
    page* it = memoryPageList->pages;
    int printLineCount = 0;
    while(it) {
        if(it->pid > 0){
            printf("|*| p[%03d] count:%02d lastUsedTime:%02f |*| \t",it->pid, it->frequency, it->lastUsedTime);
        } else{
            printf("|*| p[%03d] count:%02d lastUsedTime:%02f |*| \t",it->pid, it->frequency, it->lastUsedTime);
        }

        printLineCount++;
        if((printLineCount % 10) == 0){
            printf("\n");
        }
        it = it->nextPage;
    }
    printf("\n");
}
