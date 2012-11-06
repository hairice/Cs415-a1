/* user.c : User processes
 */

#include <xeroskernel.h>

/* Your code goes here */
void a2Process();

void producer(void) {
    /****************************/
/*
    char* thisBuff = "";
    unsigned int* pid = getProcessByPid(4)->pid;
    sysrecv(0, &thisBuff, 2);
    kprintf("received message: %s\n", thisBuff);
*/
    //syssend(4, (char*) "Hello", 8);
    
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
    sysrecv(0, &thisBuff, 5);
    kprintf("received message: %s\n", (char*) thisBuff);
*/
    
    //syssend(3, (char*) "Hello", 8);
    
    
    int i;
    //syssleep(100);

    for (i = 0; i < 5; i++) {
        kprintf("Consume %d \n", i);
        sysyield();
    }

    sysstop();
}

void root(void) {
    /****************************/

    kprintf("Process %d Root is alive!\n", sysgetpid());
    
    int first = syscreate(&a2Process, 4096);
    int second = syscreate(&a2Process, 4096);
    int third = syscreate(&a2Process, 4096);
    int fourth = syscreate(&a2Process, 4096);
    // TODO: Sleep traversal is wrong
    
    syssleep(4000);
    
    int msgBuffer = 10000;
    syssend(third, &msgBuffer, sizeof(int));
    //syscreate(&producer, 4096);
    //syscreate(&consumer, 4096);
    
    //sysyield();

/*
    for (;;) {
        sysyield();
    }
*/
}


void a2Process() {
    kprintf("Process %d is alive!\n", sysgetpid());
    syssleep(5000);
    int* sleepReceive;
    unsigned int* rootPid = getProcessByPid(1)->pid;
    kprintf("root pid: %d\n", rootPid);
    sysrecv(rootPid, sleepReceive, sizeof(int));
    kprintf("Process %d message received! sleeping for %d ms\n", 
        sysgetpid(), sleepReceive);
    kprintf("Process %d sleeping has stopped. exiting.\n");
}