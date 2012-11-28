/* user.c : User processes
 */

#include <xeroskernel.h>

/* Your code goes here */
void a2Process();

void producer(void) {
    /****************************/
/*
    int recv = 13;
    int* sleepReceive = recv;
    unsigned int* pid = getProcessByPid(4)->pid;
    kprintf(" receiving from %d\n", sysgetpid());
    int receiveStatus = sysrecv(0, &sleepReceive, sizeof(int));
    kprintf("Receipt finished! Status is %d\n", receiveStatus);
    kprintf("received message: %d\n", sleepReceive);
    
*/
/*
    int sndAmt = 10022;
    int* sndBuff = sndAmt;
    kprintf("sending %d from %d with buffer_length %d\n", sndBuff, sysgetpid(), sizeof(int));
    int sentStatus = syssend(4, sndBuff, sizeof(int));
    kprintf("Message sent! status %d\n", sentStatus);    
*/

    int i;

    for (i = 0; i < 5; i++) {
        //kprintf("Produce %d\n", i);
        sysyield();
    }
    
    sysstop();
}

void consumer(void) {
    /****************************/
    int recv = 99;
    int* sleepReceive = recv;
    unsigned int* pid = getProcessByPid(sysgetpid())->pid;
    kprintf(" receiver pid: %d\n", sysgetpid());
    int receiveStatus = sysrecv(pid, &sleepReceive, -2);
    kprintf("Receipt finished! Status is %d\n", receiveStatus);
    kprintf("received message: %d\n", sleepReceive);
    
/*
    unsigned int destPid = sysgetpid();
    int sndAmt = 10022;
    int* sndBuff = sndAmt;
    kprintf("sending %d from %d to %d with buffer_length %d\n", 
        sndBuff, sysgetpid(), destPid, sizeof(int));
    int sentStatus = syssend(sysgetpid(), sndBuff, sizeof(int));
    kprintf("Message sent! status %d\n", sentStatus);
*/
    
    int i;
    //syssleep(100);

    for (i = 0; i < 5; i++) {
      //  kprintf("Consume %d \n", i);
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
    
    int sndAmt = 10000;
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
    int retStatus = sysrecv(fourth, &msgRecv, sizeof(int));
    
    kprintf("Process %d return status from message send to %d is %d\n",
        sysgetpid(), fourth, retStatus);
    
    sndAmt = 1;
    int sendStatus = syssend(third, sndBuff, sizeof(int));
    kprintf("Process %d return status from message send to %d is %d\n",
        sysgetpid(), third, sendStatus);

*/
    
    // used for sending/receiving tests
    syscreate(&producer, 4096);
    syscreate(&consumer, 4096);
}


void a2Process() {
    kprintf("Process %d is alive!\n", sysgetpid());
    syssleep(5000);
    int sleeper = 3;
    int* sleepReceive = sleeper;
    unsigned int* rootPid = getProcessByPid(1)->pid;
    sysrecv(rootPid, &sleepReceive, sizeof(int));
    
    //kprintf("sleepRecv %d\n", &sleepReceive);
    
    kprintf("Process %d message received! sleeping for %d ms *: %d\n", 
        sysgetpid(), sleepReceive, *sleepReceive);
    
    syssleep(sleepReceive);
    
    kprintf("Process %d sleeping has stopped. exiting.\n");
}