/*
// COEN 383 AOS Project 4
// File: randomPageSelect.c
*/

#include <stdlib.h>
#include "policy.h"

// Implement Random Pick page replacement
void randomPick(memoryPages * listOfPages) {
    page* firstPage = listOfPages->pages;
    page* pageToBeReplaced = listOfPages->pages;
    int count = 0;
    int randomPickId = rand() % TOTAL_PAGES_IN_MEMORY;
    while(firstPage) {
        if(firstPage->pid>0 && count < randomPickId){
            pageToBeReplaced = firstPage;
        }
        firstPage = firstPage->nextPage;
        count++;
    }
    //Page is no longer needed
    pageToBeReplaced->pid = -1;
    pageToBeReplaced->pageNumber = -1;
}
