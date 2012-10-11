/* ctsw.c : context switcher
 */

#include <i386.h>
#include <xeroskernel.h>

void contextinit();
int contextswitch(struct pcb*);
void _ISREntryPoint();
extern int set_evec(int, long);


static void *k_stack;
static int ESP;
static int systemCallType;
static int CTSW_INTERRUPT = 57;
static int returnVar;

/* Your code goes here - You will need to write some assembly code. You must
   use the gnu conventions for specifying the instructions. (i.e this is the
   format used in class and on the slides.) You are not allowed to change the
   compiler/assembler options or issue directives to permit usage of Intel's
   assembly language conventions.
*/

void contextinit() {
	void (*entryFunc)() = &_ISREntryPoint;
	setEvec(CTSW_INTERRUPT, entryFunc);
}


extern int contextswitch(struct pcb* process) { 
	//kprintf("c");
	ESP = process->context->esp;
	
	//printContext("\nswitch", process->context);
	//kprintf("ESP: %d\n", ESP);

	__asm __volatile("\
		pushf\n\
		pusha\n\
		movl %%esp, k_stack\n\
		movl ESP, %%esp\n\
		popa\n\
		iret\n\
		\
		_ISREntryPoint:\n\
			pusha\n\
			movl %%esp, ESP\n\
			movl k_stack, %%esp\n\
			movl %%eax, returnVar\n\
			popa\n\
			popf\n\
		"
		:
		:
		: "%eax"
	);

	//kprintf("C");
	process->context->esp = ESP;
	//kprintf("Returning from ctsw: %d\n", returnVar);
	return returnVar;
}
