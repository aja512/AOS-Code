/*
// COEN 383 AOS Project 4
// File: fifo.c
*/
#include <stdio.h>
#include <stdlib.h>
#include "policy.h"

// Implement FIFO page replacement
void firstInFirstOut(memoryPages * listOfPages)
{
  page* firstPage = listOfPages->pages;
  page* pageToBeReplaced = listOfPages->pages;
  
  // iterate through process list
  while(firstPage) 
  {
     if(firstPage->pageAllocatedTime < pageToBeReplaced->pageAllocatedTime) 
     {
            pageToBeReplaced = firstPage;
     }
     firstPage = firstPage->nextPage;
  }
   if (DEBUG==1) printf("EVICTED Ones :: p[%03d] c:%02d l:%02f\n", pageToBeReplaced->pid, pageToBeReplaced->pageNumber, pageToBeReplaced->lastUsedTime);
  
  //Page is no longer needed
  pageToBeReplaced->pid = -1;
  pageToBeReplaced->pageNumber = -1;
}
