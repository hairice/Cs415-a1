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
        
    kprintf("1st %d, 2nd: %d, 3rd %d, 4th %d\n", 
        first, second, third, fourth);
    
    //syssleep(4000);
    
    int sndAmt = 10000;
    int* sndBuff = sndAmt;
    //syssend(third, (int*) 10000, sizeof(int));
    syssend(third, sndBuff, sizeof(int));
    
    sndAmt = 7000;
    //syssend(second, (int*) 7000, sizeof(int));
    sndAmt = 20000;
    //syssend(first, (int*) 20000, sizeof(int));
    sndAmt = 27000;
    //syssend(fourth, (int*) 27000, sizeof(int));
    
    //syscreate(&producer, 4096);
    //syscreate(&consumer, 4096);
}


void a2Process() {
    kprintf("Process %d is alive!\n", sysgetpid());
    //syssleep(5000);
    int sleeper = 1;
    int* sleepReceive = sleeper;
    unsigned int* rootPid = getProcessByPid(1)->pid;
    sysrecv(rootPid, &sleepReceive, 8);
    kprintf("Process %d message received! sleeping for %d ms\n", 
        sysgetpid(), sleepReceive);
    
    syssleep(sleepReceive);
    
    kprintf("Process %d sleeping has stopped. exiting.\n");
}