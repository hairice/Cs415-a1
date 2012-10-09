/* ctsw.c : context switcher
 */

#include <i386.h>
#include <xeroskernel.h>

void contextinit();
int contextswitch(void);
void _ISREntryPoint();
extern int set_evec(int, long);


static void *k_stack;
static int ESP;
static int systemCallType;
static int CTSW_INTERRUPT = 57;

/* Your code goes here - You will need to write some assembly code. You must
   use the gnu conventions for specifying the instructions. (i.e this is the
   format used in class and on the slides.) You are not allowed to change the
   compiler/assembler options or issue directives to permit usage of Intel's
   assembly language conventions.
*/

void contextinit() {
	kprintf("set_evec\n");
	setEvec(CTSW_INTERRUPT, &_ISREntryPoint);
}


extern int contextswitch() { 
	kprintf("c");
	struct pcb* process = next();
	extern long initsp;
	// k_stack = initsp;
	ESP = process->context->esp;
	
	printContext("\nswitch", process->context);
	kprintf("ESP: %d\n", ESP);

	int i;
	for (i=0; i< 20000000; i++) {
		;
	}

	// add an int CTSW_INTERRUPT instruction?
	__asm __volatile(" \
		pushf\n \
		pusha\n \
		movl %%esp, k_stack\n \
		movl ESP, %%esp\n \
		popa\n \
		iret\n \
		\
		_ISREntryPoint:\n \
			pusha\n \
			movl %%esp, ESP\n \
			movl k_stack, %%esp\n \
			popa\n \
			popf\n \
		"
		:
		:
		: "%eax"
	);
	//trap2(CTSW_INTERRUPT);
	kprintf("C");
	process->context->esp = ESP;
	return ESP;
}
