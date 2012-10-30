/* create.c : create a process
 */

#include <xeroskernel.h>
#include <xeroslib.h>

pcb     proctab[MAX_PROC];

/* make sure interrupts are armed later on in the kernel development  */
#define STARTING_EFLAGS         0x00003000

static int      nextpid = 1;



int      create( funcptr fp, int stackSize ) {
/***********************************************/

    context_frame       *contextFrame;
    pcb                 *process = NULL;
    int                 i;

    if( stackSize < PROC_STACK ) {
        stackSize = PROC_STACK;
    }

    for( i = 0; i < MAX_PROC; i++ ) {
        if( proctab[i].state == STATE_STOPPED ) {
            process = &proctab[i];
            break;
        }
    }

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
    
    
    process->esp = (int)contextFrame;
    process->state = STATE_READY;
    process->pid = nextpid++;
    
    kprintf("ebp: %d, &ebp: %d, esp: %d, &esp: %d\n", 
        contextFrame->ebp, &contextFrame->ebp, contextFrame->esp, &contextFrame->esp);
    
    // TODO: Do I need to subtract the context size or anything? Related to the magic #132?
    process->stackSize = stackSize;

    ready( process );
    return( process->pid );
}
