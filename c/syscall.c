/* syscall.c : syscalls
 */

#include <xeroskernel.h>
#include <stdarg.h>

/* Your code goes here */

extern int syscall(int call, ...);
extern int syscreate( void (*func)(), int stack );
extern void sysyield( void );
extern void sysstop( void );

static int CALL;
static va_list ARGS;

extern int syscall(int call, ...) {
	va_list args;
    va_start(args, call);
	int syscallReturn;
	CALL = call;
	ARGS = args;

	kprintf("In syscall!\n");

	__asm __volatile("\
		movl CALL, %%eax \n \
		movl ARGS, %%edx \n \
		#int $58 \n \
		movl %%eax, %0 \n \
	"
	: "=r" (syscallReturn)
	: 
	: "%eax");

	va_end(args);

	kprintf("syscallReturn: %d\n", syscallReturn);	
	return syscallReturn;
}

/**
	Takes a function pointer denoting the address to start execution at
 	and an integer that denotes the size of the new process’s stack. The
 	function returns the process ID of the created process.
*/
extern int syscreate(void (*func)(), int stack) {
	return syscall(CREATE, func, stack);
}

extern void sysyield(void) {
	syscall(YIELD);
}
extern void sysstop(void) {
	syscall(STOP);
}
