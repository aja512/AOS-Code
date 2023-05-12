/*
// COEN 383 AOS Project 4
// File: mfu.c
*/

#include <stdio.h>
#include <stdlib.h>
#include "policy.h"

// Implement MFU page replacement

void MostFrequentlyUsed(memoryPages * listOfPages) 
{
  page* firstPage = listOfPages->pages; //allocate starting of the queue
  page* pageToBeReplaced = listOfPages->pages;
  int max = firstPage->frequency; // set max to page frequency 
  
  // iterate through process list
  
  while(firstPage) 
  {
    if(firstPage->frequency > max)
    {
      pageToBeReplaced = firstPage;
      max = firstPage->frequency;
    }
    firstPage = firstPage -> nextPage;
  }
  if (DEBUG==1) printf("EVICTED: p[%03d] c:%02d l:%02f\n", pageToBeReplaced->pid, pageToBeReplaced->pageNumber, pageToBeReplaced->lastUsedTime);
  
  //Page is no longer needed
  pageToBeReplaced->pid = -1;
  pageToBeReplaced->pageNumber = -1;
}  
