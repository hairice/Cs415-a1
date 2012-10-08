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

/* Your code goes here - You will need to write some assembly code. You must
   use the gnu conventions for specifying the instructions. (i.e this is the
   format used in class and on the slides.) You are not allowed to change the
   compiler/assembler options or issue directives to permit usage of Intel's
   assembly language conventions.
*/

void contextinit() {
	setEvec(57, &_ISREntryPoint);
}


extern int contextswitch(struct pcb* process) { 
	kprintf("c");
	extern long initsp;
	k_stack = initsp;
	ESP = (int) process->context->esp;
	kprintf("\nEsp: %d, kstack: %d\n", ESP, k_stack);

	int i;
	for (i=0; i< 10000000; i++) {
		;
	}	

	__asm __volatile("pushf\n \
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

	kprintf("C");
	process->context->esp = ESP;
	return ESP;
}
