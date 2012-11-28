/* create.c : create a process
 */

#include <xeroskernel.h>
#include <xeroslib.h>

pcb* getNextProcess();

pcb     proctab[MAX_PROC];

/* make sure interrupts are armed later on in the kernel development  */
#define STARTING_EFLAGS         0x00003200

static int      nextpid = 1;


/**
	Takes two parameters, a reference (function pointer) to the start 
	of the process, and an integer denoting the amount of stack to allocate for the 
	process. Returns the new process’ pid on success and −1 on failure
*/
int      create( funcptr fp, int stackSize ) {
/***********************************************/

    context_frame       *contextFrame;
    pcb                 *process = NULL;

    if( stackSize < PROC_STACK ) {
        stackSize = PROC_STACK;
    }

    process = getNextProcess();

    if( !process ) {
        return( -1 );
    }

    contextFrame = kmalloc(stackSize * 2);

    if( !contextFrame ) {
        return( -1 );
    }

    contextFrame = (context_frame *)((int)contextFrame + stackSize - 4);
    contextFrame--;
    
    memset(contextFrame, 0x81, sizeof( context_frame ));
    
    contextFrame->iret_cs = getCS();
    contextFrame->iret_eip = (unsigned int)fp;
    contextFrame->eflags = STARTING_EFLAGS;

    contextFrame->esp = (int)(contextFrame + 1);
    contextFrame->ebp = contextFrame->esp;
    unsigned int* procStackPtr = (unsigned int*) contextFrame->ebp;
    *procStackPtr = getSysStopAddr();
    //procStackPtr++;
    
    process->esp = (int)contextFrame;
    process->state = STATE_READY;
    
    // move this into getNextProcess()?
    if (nextpid > MAX_PROC) {
        process->pid += MAX_PROC;
    }
    
    nextpid++;
    process->senderQueue = 0;
    
   // kprintf("ebp: %d, &ebp: %d, esp: %d, &esp: %d, systop: %d\n", 
   //     contextFrame->ebp, &contextFrame->ebp, contextFrame->esp, &contextFrame->esp, getSysStopAddr());
    
    // TODO: Do I need to subtract the context size or anything? Related to the magic #132?
    process->stackSize = stackSize;

    if (fp != idleproc) {
        ready(process);
    }
    
    return( process->pid );
}

pcb* getNextProcess() {
    return getProcessByPid(nextpid);
}


/**
 * Given a pid, returns the corresponding process
 */
extern pcb* getProcessByPid(unsigned int pid) {
    int index = (pid % MAX_PROC) - 1;
    if (index < 0) index = MAX_PROC;
    return &proctab[index];
}