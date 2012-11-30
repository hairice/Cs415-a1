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

void     root() {
    char  buff[100];
    sysputs("Root has been called\n");
    
    // syssighandler testing

    unsigned int* oldHandler;
    syssighandler(1, &testOldHandler, &oldHandler);
    int ret = syskill(sysgetpid(), 1);
    kprintf("Back in the root function after syskill(). Result = %d\n", ret);
    syssighandler(1, &testHandler, &oldHandler);
    
    
    int i;
    for(i = 0; i < 100000000; i++);

    sysyield();
    sysyield();
   
    syscreate( &producer, 4096 );
    syscreate( &consumer, 4096 );
    sprintf(buff, "Root finished\n");
    sysputs( buff );
    sysstop();
}
