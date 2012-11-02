/* user.c : User processes
 */

#include <xeroskernel.h>

/* Your code goes here */
void producer(void) {
    /****************************/
    char* thisBuff = "";
    unsigned int* pid = getProcessByPid(3)->pid;
    sysrecv(pid, &thisBuff, 5);
    kprintf("received message: %s\n", thisBuff);
/*
    syssend(3, (char*) "Hello", 8);
*/
    
    kprintf("back producing\n");

    int i;

    for (i = 0; i < 5; i++) {
        kprintf("Produce %d\n", i);
        sysyield();
    }

    sysstop();
}

void consumer(void) {
    /****************************/
/*
    char* thisBuff = "";
    unsigned int* pid = getProcessByPid(2)->pid;
    sysrecv(pid, &thisBuff, 5);
    kprintf("received message: %s\n", (char*) thisBuff);
*/
    
    syssend(2, (char*) "Hello", 8);
    
    
    int i;

    for (i = 0; i < 5; i++) {
        kprintf("Consume %d \n", i);
        sysyield();
    }

    sysstop();
}

void root(void) {
    /****************************/

    kprintf("Root has been called\n");
    sysyield();
    sysyield();
    syscreate(&producer, 4096);
    syscreate(&consumer, 4096);


    for (;;) {
        sysyield();
    }
}
