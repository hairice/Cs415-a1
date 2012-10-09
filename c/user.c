/* user.c : User processes
 */

#include <xeroskernel.h>

/* Your code goes here */

extern void createRootProcess();
extern void root( void );

extern void createRootProcess() {
	syscreate(&root, 256);
}

extern void root( void ) {
	kprintf("Hello World\n");

	for (;;) {
		sysyield();
	}
}
