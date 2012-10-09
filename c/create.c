/* create.c : create a process
 */

#include <xeroskernel.h>

/* Your code goes here. */
int create(void (*pfunc)(), int stackSize);
struct pcb* allocatePcb();


extern struct pcb;
extern struct processContext;

/**
	Takes two parameters, a reference (function pointer) to the start 
	of the process, and an integer denoting the amount of stack to allocate for the 
	process. Returns the new process’ pid on success and −1 on failure

*/
int create(void (*pfunc)(), int stackSize) {
	struct pcb* pcb = getFreeProcess();
	//struct pcb* pcb = (struct pcb*) pfunc;	

	if (!pcb) {
		return -1;
	}

	// Allocate the stack using kmalloc()
	struct processContext* context = kmalloc(stackSize + sizeof(struct processContext));
	
	// Initialize the pcb and the stack
	context->eip = pfunc;
	initializeContext(context, stackSize);
	pcb->context = context;
	printContext("\nnewly created", pcb->context);
	
	// Place the process on the ready queue
	ready(pcb);

	return pcb->pid;
}

void initializeContext(struct processContext* context, int stackSize) {
	kprintf("Initializing Process!\n");
	context->edi = 0;
	context->esi = 0;
	
	context->ebp = (unsigned int) context + sizeof(struct processContext) + (stackSize * (3/4));
	context->esp = (unsigned int) context + sizeof(struct processContext) + (stackSize * (3/4));

	//context->ebp = (unsigned int) context;
	//context->esp = (unsigned int) context;
	//context->ebp = 0;
	//context->esp = 0;
	
	context->ebx = 0;
	context->edx = 0;
	context->ecx = 0;
	context->eax = 0;

	context->cs = getCS();
	context->eflags = 0;
}

extern void printContext(char* msg, struct processContext* context) {
	kprintf("%s: edi: %d esi: %d ebp: %d esp: %d ebx: %d edx: %d ecx: %d eax: %d eip: %d\n", msg, 
	context->edi, context->esi, context->ebp, context->esp, context->ebx,
	context->edx, context->ecx, context->eax, context->eip); 
}
