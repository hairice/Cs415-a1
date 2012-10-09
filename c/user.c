/* user.c : User processes
 */

#include <xeroskernel.h>

/* Your code goes here */

extern void createRootProcess();
extern void root( void );

extern void createRootProcess() {
	kprintf("Creating root process\n");
	syscreate(&root, 256);
}

extern void root( void ) {
	kprintf("\n---------------------\n");
	kprintf("Hello World\n");

	for (;;) {
		sysyield();
	}
}
