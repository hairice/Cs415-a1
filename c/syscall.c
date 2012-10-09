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
static int SYSCALL_RETURN;

extern int syscall(int call, ...) {
	va_list args;
    va_start(args, call);
	CALL = call;
	ARGS = args;

	kprintf("In syscall!\n");
	// The int instr doesn't work.

	switch (call) {
		case (CREATE): {
			void* func = va_arg(args, void*);
			int size = va_arg(args, int);
			create(func, size);
			break;
		}
		case (YIELD): break;
		case (STOP): break;
	} 

	va_end(args);
	trap2(57);
	__asm __volatile("\
		movl CALL, %%eax\n\
		movl ARGS, %%edx\n\
		int $57\n\
		movl %%eax, SYSCALL_RETURN\n\
	"
	:
	: 
	: "%eax", "%edx");

	kprintf("syscallReturn: %d\n", SYSCALL_RETURN);	
	return SYSCALL_RETURN;
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
