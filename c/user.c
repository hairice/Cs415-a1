/* user.c : User processes
 */

#include <xeroskernel.h>

/* Your code goes here */
void a2Process();

void producer(void) {
    /****************************/
    int recv = 3;
    int* sleepReceive = recv;
    unsigned int* pid = getProcessByPid(4)->pid;
    kprintf(" receiving from %d\n", sysgetpid());
    sysrecv(4, &sleepReceive, 2);
    kprintf("received message: %d\n", sleepReceive);
    
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
    
    int sndAmt = 10022;
    int* sndBuff = sndAmt;
    kprintf("sending from %d\n", sysgetpid());
    syssend(3, sndBuff, sizeof(int));
    
    
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
    
/*
    int first = syscreate(&a2Process, 4096);
    int second = syscreate(&a2Process, 4096);
    int third = syscreate(&a2Process, 4096);
    int fourth = syscreate(&a2Process, 4096);
    // TODO: Sleep traversal is wrong
        
    kprintf("1st %d, 2nd: %d, 3rd %d, 4th %d\n", 
        first, second, third, fourth);
    
    syssleep(4000);
    
    int sndAmt = 10022;
    int* sndBuff = sndAmt;
    syssend(third, sndBuff, sizeof(int));
    
    sndAmt = 7000;
    syssend(second, sndBuff, sizeof(int));
    sndAmt = 20000;
    syssend(first, sndBuff, sizeof(int));
    sndAmt = 27000;
    syssend(fourth, sndBuff, sizeof(int));
    
    int message = 1;
    int* msgRecv = message;
    int retStatus = sysrecv(fourth, msgRecv, sizeof(int));
    
    kprintf("Process %d return status from message send to %d is %d\n",
        sysgetpid(), fourth, retStatus);
    
    sndAmt = 1;
    int sendStatus = syssend(third, sndBuff, sizeof(int));
    kprintf("Process %d return status from message send to %d is %d\n",
        sysgetpid(), third, sendStatus);
*/
    
    syscreate(&producer, 4096);
    syscreate(&consumer, 4096);
}


void a2Process() {
    kprintf("Process %d is alive!\n", sysgetpid());
    syssleep(5000);
    int sleeper = 3;
    int* sleepReceive = sleeper;
    unsigned int* rootPid = getProcessByPid(1)->pid;
    sysrecv(rootPid, &sleepReceive, 8);
    
    kprintf("sleepRecv %d\n", &sleepReceive);
    
    kprintf("Process %d message received! sleeping for %d ms *: %d\n", 
        sysgetpid(), sleepReceive, *sleepReceive);
    
    //syssleep(sleepReceive);
    
    kprintf("Process %d sleeping has stopped. exiting.\n");
}