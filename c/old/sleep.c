/* sleep.c : sleep device (assignment 2)
 */

#include <xeroskernel.h>

/* Your code goes here */

static pcb* sleepHead = NULL;

/**
 * Places the current process in a list of sleeping processes such
 * that its place in the list corresponds to when it needs to be woken
 * @param ticks is the number of ticks that the current process should sleep for
 */
extern void sleep(int ticks) {
    //kprintf("in sleep function\n");

    pcb* currProcess = getCurrentProcess();
    currProcess->sleepDelta = ticks;
    
    // Place the current process in the sleep queue
    if (sleepHead) {
        if (sleepHead->sleepDelta <= currProcess->sleepDelta) {
            pcb* pqueuePtr = sleepHead;
            
            while (pqueuePtr) {
/*
                kprintf("subtracting delta %d = %d - %d\n", 
                    currProcess->sleepDelta - pqueuePtr->sleepDelta,
                    currProcess->sleepDelta, pqueuePtr->sleepDelta);
*/
                currProcess->sleepDelta -= pqueuePtr->sleepDelta ;
                
                if (!pqueuePtr->next || pqueuePtr->next->sleepDelta >= currProcess->sleepDelta) {
                    break;
                } else {
                    pqueuePtr = pqueuePtr->next;
                }
            }
            
            if (!pqueuePtr->next) {
/*
                kprintf("No next: currDelta %d, pqueue Delta: %d\n",
                    currProcess->sleepDelta, pqueuePtr->sleepDelta);
*/
                pqueuePtr->next = currProcess;
                currProcess->prev = pqueuePtr;
            } else {
                //kprintf("Adding: currDelta %d, pqueue Delta: %d\n", currProcess->sleepDelta, pqueuePtr->sleepDelta);
                currProcess->next = pqueuePtr;
                currProcess->prev = pqueuePtr->prev;
                pqueuePtr->sleepDelta = currProcess->sleepDelta - pqueuePtr->sleepDelta;
                pqueuePtr->prev->next = currProcess;
                pqueuePtr->prev = currProcess;
            }
                  
        } else {
            // sleepHead->SleepDelta > currProcess->sleepDelta
            
            //kprintf("in else... %d - %d\n", sleepHead->sleepDelta, currProcess->sleepDelta);
            sleepHead->sleepDelta -= currProcess->sleepDelta;
            sleepHead->prev = currProcess;
            currProcess->next = sleepHead;
            sleepHead = currProcess;
        }
        
    } else {
        // There is no sleepHead
        
        //kprintf("in sleep else - ticks %d, delta %d\n", ticks, currProcess->sleepDelta);
        currProcess->next = NULL;
        currProcess->prev = NULL;
        sleepHead = currProcess;
    }

    //testTraverseSleepQueue();
}

/**
 * Notifies the sleep device code that a clock tick has occurred
 */
extern void tick() {
    if (sleepHead) {
        sleepHead->sleepDelta--;
        //kprintf("  -    tick! %d: %d     - ", sleepHead->pid, sleepHead->sleepDelta);
    }
    
    if (sleepHead->sleepDelta < 1) {
        //kprintf("!!!!!!!!!!unslept\n");
        pcb* nextHead = sleepHead->next;
        ready(sleepHead);
        sleepHead = nextHead;
    }
}

void testTraverseSleepQueue() {
    pcb* currentItem = sleepHead;
    while (currentItem->next) {
        kprintf("current sleep pid %d delta: %d next: %d\n", 
            currentItem->pid, currentItem->sleepDelta, currentItem->next->pid);
        currentItem = currentItem->next;
    }
    kprintf("FINAL     current sleep pid %d delta: %d\n", currentItem->pid, currentItem->sleepDelta);
}