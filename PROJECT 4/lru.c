/*
// COEN 383 AOS Project 4
// File: lru.c
*/

#include <stdio.h>
#include <stdlib.h>
#include "policy.h"

// Implement LRU page replacement
void leastRecentlyUsed(memoryPages *listOfPages)
{
  page* firstPage = listOfPages->pages; //allocate starting of the queue
  page* pageToBeReplaced = listOfPages->pages;
  int lru = firstPage->lastUsedTime; // set lru to Last Used Time
  
  // iterate through process list
 
  while(firstPage) 
  {
    if(firstPage->lastUsedTime < lru)
        {
            pageToBeReplaced = firstPage;
            lru = firstPage->lastUsedTime;
        }
        firstPage = firstPage->nextPage;
  }
  
  if (DEBUG==1) printf("EVICTED: p[%03d] c:%02d l:%02f\n", pageToBeReplaced->pid, pageToBeReplaced->pageNumber, pageToBeReplaced->lastUsedTime);
  
  //Page is no longer needed
  pageToBeReplaced->pid = -1;
  pageToBeReplaced->pageNumber = -1;
}
