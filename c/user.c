/* user.c : User processes
 */


#include <xeroskernel.h>
#include <xeroslib.h>

void producer() {
    int         i;
    char        buff[100];

    for( i = 0; i < 20; i++ ) {

      sprintf(buff, "Producer %d and in hex %x\n", i, i);
      sysputs(buff);
      //sysyield();
    }
    for (i = 0; i < 20; i++) {
      sysputs("P");
      syssleep(1500);
    }
    sprintf(buff, "Producer finished\n");
    sysputs( buff );
    sysstop();
}

void consumer() {
    int         i;
    char        buff[100];

    for( i = 0; i < 10; i++ ) {
      sprintf(buff, "Consume %d\n", i);
      sysputs( buff );
      sysyield();
    }

    for (i = 0; i < 20; i++) {
      sysputs("C");
      syssleep(1000);
    }

    sprintf(buff, "Consumer finished\n");
    sysputs( buff );
    sysstop();
}

void     a2root() {
    char  buff[100];
    sysputs("Root has been called\n");
    
    // syssighandler testing

    unsigned int* oldHandler;
    syssighandler(1, &testOldHandler, &oldHandler);
    int ret = syskill(sysgetpid(), 1);
    kprintf("Back in the root function after syskill(). Result = %d\n", ret);
    syssighandler(1, &testHandler, &oldHandler);
    //di_open(KBD_NONECHO);
    
    kprintf("Back in root\n");
/*
    
    int i;
    for(i = 0; i < 1000000; i++);

    sysyield();
    sysyield();
   
    syscreate( &producer, 4096 );
    syscreate( &consumer, 4096 );
    sprintf(buff, "Root finished\n");
    sysputs( buff );
*/
    sysstop();
}

void root() {
    sysputs("Hello, I am Root\n");
    int fd = sysopen(KBD_ECHO);
    char buff[100];
    sysputs("Reading bytes\n");
    sysread(fd, buff, 4);
    kprintf("read: %s\n", buff);
    sysstop();
}
