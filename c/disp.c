/* disp.c : dispatcher
 */

#include <xeroskernel.h>
#include <xeroslib.h>
#include <stdarg.h>


void updateCurrentProcess(pcb* process);
unsigned int getContextMemLoc(pcb* process);

static pcb *readyHead = NULL;
static pcb *readyTail = NULL;
static pcb* currentProcess = NULL;

void dispatch(void) {
    /********************************/

    pcb *pcb;
    int ctswNumber;
    funcptr fp;
    int stack;
    va_list args;

    for (pcb = next(); pcb;) {
        //      kprintf("Process %x selected stck %x\n", p, p->esp);

        ctswNumber = contextswitch(pcb);
        switch (ctswNumber) {
            case( SYS_CREATE):
                args = (va_list) pcb->args;
                fp = (funcptr) (va_arg(args, int));
                stack = va_arg(args, int);
                pcb->ret = create(fp, stack);
                break;
            case( SYS_YIELD):
                ready(pcb);
                pcb = next();
                break;
            case( SYS_STOP):
                pcb->state = STATE_STOPPED;
                cleanup(pcb);
                pcb = next();
                break;
            default:
                kprintf("Bad Sys request %d, pid = %d\n", ctswNumber, pcb->pid);
        }
    }

    kprintf("Out of processes: dying\n");

    for (;;);
}

extern void dispatchinit(void) {
    /********************************/

    //bzero( proctab, sizeof( pcb ) * MAX_PROC );
    memset(proctab, 0, sizeof ( pcb) * MAX_PROC);
}

extern void ready(pcb *p) {
    /*******************************/

    p->next = NULL;
    p->state = STATE_READY;

    if (readyTail) {
        readyTail->next = p;
    } else {
        readyHead = p;
    }

    readyTail = p;
}

/**
 * Returns the next pcb in the readyQueue and updates the current process
 */
extern pcb *next(void) {
    /*****************************/

    pcb *p;

    p = readyHead;

    if (p) {
        readyHead = p->next;
        if (!readyHead) {
            readyTail = NULL;
        }
    }

    updateCurrentProcess(p);

    return ( p);
}

void updateCurrentProcess(pcb* process) {
    currentProcess = process;
}

extern unsigned int getCurrentPid() {
    return currentProcess->pid;
}

void cleanup(pcb* process) {    
    context_frame* context = process->esp;
    unsigned int contextMemLoc = getContextMemLoc(process);

/*
    kprintf("process->esp: %d, -stackSize: %d, contextMemLoc: %d\n", 
        process->esp, process->esp - process->stackSize, contextMemLoc);

    kprintf("edi: %d, esi: %d, ebp: %d, esp: %d, eflags: %d, cs: %d\n",
            context->edi, context->esi, context->ebp, context->esp,
            context->eflags, context->iret_cs);
*/    
    kprintf("cleaning ");
    
    kfree((void*)contextMemLoc);
}

unsigned int getContextMemLoc(pcb* process) {
    return process->esp - process->stackSize + 132; //TODO: Why 132?
}