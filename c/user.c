/* user.c : User processes
 */

#include <xeroskernel.h>

/* Your code goes here */

extern void createRootProcess();
extern void root( void );
extern void producer();
extern void consumer();


extern void createRootProcess() {
	kprintf("Creating root process\n");
	create(&root, 256);
}

extern void root( void ) {
	kprintf("\nHello World\n");
	
	syscreate(&producer, 256);
	syscreate(&consumer, 256);

	for (;;) {
		sysyield();
	}
}

void producer() {
	int i;
	for (i = 0; i < 12; i++) {
		kprintf("Happy");
		sysyield();
	}

	for (i= 0; i < 10000000; i++);

	sysstop();
}

void consumer() {
	int i;
	for (i = 0; i < 15; i++) {
		kprintf("New Year");
		sysyield();
	}

	sysstop();	
}
