/*
// COEN 383 AOS Project 4
// File: lfu.c
*/

#include <stdio.h>
#include "policy.h"

// Implement LFU page replacement
void leastFrequentlyUsed(memoryPages* pageList) {
    page *current = pageList->pages;    // Set current to page list head
    page *target = pageList->pages;     // Set target to page list head
    int min_freq = current->frequency;

    while (current != NULL) {
        if(current->frequency < min_freq) {     // Change target page if it has lower frequency
            target = current;
            min_freq = current->frequency;
        }
        current = current->nextPage;
    }
    // Print the information of the evicted page
    if (target->pid < 123)  printf("Evicted page: pid=%c, freq=%d\n", (char) target->pid, target->frequency);
    else    printf("Evicted page: pid=%d, freq=%d\n", target->pid, target->frequency);

    // Mark page as empty as it was evicted
    target->pid = -1;
    target->pageNumber = -1;
    target->frequency = 0;
}
