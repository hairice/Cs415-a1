/* user.c : User processes
 */

#include <xeroskernel.h>

/* Your code goes here */

extern void createRootProcess();
extern void root( void );
extern void producer();
extern void consumer();


extern void createRootProcess() {
	//kprintf("Creating root process\n");
	void (*rootProcess)() = &root;
	create(rootProcess, 256);
}

extern void root( void ) {
	kprintf("\nHello World\n");

//	kprintf("Creating producer\n");
	syscreate(&producer, 256);

//	kprintf("Creating consumer\n");
	syscreate(&consumer, 256);

	for (;;) {
//		kprintf("\nProducer(): %d; Consumer(): %d\n", &producer, &consumer);
//		kprintf("Root Yielding\n");
		sysyield();
	}
}

void producer() {
	int i;
	for (i = 0; i < 12; i++) {
		kprintf("Happy\n");
		sysyield();
	}

	sysstop();
}

void consumer() {
	int i;
	for (i = 0; i < 15; i++) {
		kprintf("New Year\n");
		sysyield();
	}

	sysstop();	
}
