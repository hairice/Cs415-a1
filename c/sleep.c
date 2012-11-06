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
    kprintf("in sleep function\n");

    // delta list    
    // http://everything2.com/title/delta+list
    pcb* currProcess = getCurrentProcess();
    currProcess->sleepDelta = ticks;
    currProcess->state = STATE_BLOCKED;
    
    if (sleepHead) {
        if (sleepHead->sleepDelta <= currProcess->sleepDelta) {
            pcb* pqueuePtr = sleepHead;
            currProcess->sleepDelta -= pqueuePtr->sleepDelta;
            while (pqueuePtr->next && 
                    pqueuePtr->next->sleepDelta <= currProcess->sleepDelta) {
                currProcess->sleepDelta -= pqueuePtr->sleepDelta;
                pqueuePtr = pqueuePtr->next;
            }
            
            if (!pqueuePtr->next) {
                currProcess->sleepDelta -= pqueuePtr->sleepDelta;
                pqueuePtr->next = currProcess;
                currProcess->prev = pqueuePtr;
                currProcess->next = NULL;
            } else {
                currProcess->next = pqueuePtr;
                currProcess->prev = pqueuePtr->prev;
                pqueuePtr->prev->next = currProcess;
                pqueuePtr->prev = currProcess;
            }
                  
        } else {
            sleepHead->prev = currProcess;
            currProcess->next = sleepHead;
            sleepHead = currProcess;
        }
    } else {
        // There is a problem in here...
        kprintf("in sleep else - ticks %d, delta %d\n", ticks, currProcess->sleepDelta);
        currProcess->next = NULL;
        currProcess->prev = NULL;
        sleepHead = currProcess;
    }
    
    testTraverseSleepQueue();
}

/**
 * Notifies the sleep device code that a clock tick has occurred
 */
extern void tick() {
    if (sleepHead) {
        sleepHead->sleepDelta--;
        //kprintf("%d ", sleepHead->sleepDelta);
    }
    
    if (sleepHead->sleepDelta <= 0) {
        //kprintf("unslept\n");
        pcb* nextHead = sleepHead->next;
        ready(sleepHead);
        sleepHead = nextHead;
    }
}

void testTraverseSleepQueue() {
    pcb* currentItem = sleepHead;
    while (currentItem->next) {
        kprintf("current sleep pid %d delta: %d\n", currentItem->pid, currentItem->sleepDelta);
        currentItem = currentItem->next;
    }
    kprintf("current sleep pid %d delta: %d\n", currentItem->pid, currentItem->sleepDelta);
}