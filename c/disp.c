/* disp.c : dispatcher
 */

#include <xeroskernel.h>
#include <xeroslib.h>
#include <stdarg.h>


void updateCurrentProcess(pcb* process);
unsigned int getContextMemLoc(pcb* process);


static pcb* currentProcess = NULL;

static pcb *readyHead = NULL;
static pcb *readyTail = NULL;


void dispatch(void) {
    /********************************/

    pcb *pcb;
    int ctswNumber;
    funcptr fp;
    int stack;
    va_list args;
    
    void* buffer;
    int buffer_len;

    for (pcb = next(); pcb;) {
        //      kprintf("Process %x selected stck %x\n", p, p->esp);

        ctswNumber = contextswitch(pcb);
        switch (ctswNumber) {
            case(SYS_CREATE):
                args = (va_list) pcb->args;
                fp = (funcptr) (va_arg(args, int));
                stack = va_arg(args, int);
                pcb->ret = create(fp, stack);
                break;
            case(SYS_YIELD):
                ready(pcb);
                pcb = next();
                break;
            case(SYS_STOP):
                pcb->state = STATE_STOPPED;
                cleanup(pcb);
                pcb = next();
                break;
            case(TIMER_INT):
                if (pcb->state != STATE_BLOCKED) ready(pcb);
                pcb = next();
                end_of_intr();
                tick();
                break;
            case (SYS_SLEEP):
                args = (va_list) pcb->args;
                int ticks = va_arg(args, int);
                sleep(ticks);
                break;
            case(SYS_SEND):
                args = (va_list) pcb->args;
                unsigned int receiverPid = va_arg(args, unsigned int);
                buffer = va_arg(args, void*);
                buffer_len = va_arg(args, int);
                
                pcb->ret = send(receiverPid, buffer, buffer_len, pcb);
                kprintf("back in send - current pcb is %d, ", pcb->pid);
                

                if (pcb->state == STATE_BLOCKED) {
                    pcb = next();
                } else {
                  ready(pcb);
                  pcb = next();
                }
                
                kprintf("next pcb is: %d real next: %d\n", pcb->pid, pcb->next->pid);
                break;
                
            case(SYS_RECEIVE):
                args = (va_list) pcb->args;
                unsigned int* fromPid = va_arg(args, unsigned int*);    
                buffer = va_arg(args, void*);
                buffer_len = va_arg(args, int);
                
                pcb->ret = recv(pcb, fromPid, buffer, buffer_len);
                kprintf("Back in disp receive - current pcb is %d, ", pcb->pid);
                

                if (pcb->state == STATE_BLOCKED) {
                    kprintf("State is blocked for %d\n", pcb->pid);
                    pcb = next();
                } else {
                    kprintf("State is not blocked for %d\n", pcb->pid);
                    ready(pcb);
                    pcb = next();
                }
                
                kprintf("next pcb is %d, next next %d, and next next pid: %d\n", 
                        pcb->pid, pcb->next, pcb->next->pid);
                break;
            default:
                kprintf("Bad Sys request %d, pid = %d, ret %d\n", ctswNumber, pcb->pid, pcb->ret);
        }
    }

    kprintf("Out of processes: dying\n");

    for (;;);
}

extern void dispatchinit(void) {
    /********************************/

    //bzero( proctab, sizeof( pcb ) * MAX_PROC );
    memset(proctab, 0, sizeof ( pcb) * MAX_PROC);
    
    int i;
    for (i = 1; i <= MAX_PROC; i++) {
        pcb* proc = &proctab[i - 1];
        proc->pid = i;
    }
}

extern void ready(pcb *p) {
    /*******************************/
    
    p->next = NULL;
    p->prev = NULL;
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
    } else {
        p = idleProcess;
    }

    updateCurrentProcess(p);

    return (p);
}

extern void block(pcb *p) {
    /*******************************/
    p->state = STATE_BLOCKED;
    p->next = NULL;
    p->prev = NULL;
    
    kprintf("post-block traversal\n");
    testTraverseQueue(readyHead);
}

void updateCurrentProcess(pcb* process) {
    currentProcess = process;
}

extern unsigned int getCurrentPid() {
    return currentProcess->pid;
}

extern pcb* getCurrentProcess() {
    return currentProcess;
}

void cleanup(pcb* process) {
    // TODO: if process stops while sending/receiving...
    unsigned int contextMemLoc = getContextMemLoc(process);

/*
    kprintf("process->esp: %d, -stackSize: %d, contextMemLoc: %d\n", 
        process->esp, process->esp - process->stackSize, contextMemLoc);

    kprintf("edi: %d, esi: %d, ebp: %d, esp: %d, eflags: %d, cs: %d\n",
            context->edi, context->esi, context->ebp, context->esp,
            context->eflags, context->iret_cs);
  
    kprintf("cleaning ");
*/    
    kfree((void*)contextMemLoc);
}

unsigned int getContextMemLoc(pcb* process) {
    return process->esp - process->stackSize + 132; //TODO: Why 132?
}

extern void idleproc() {
    int i;
    for (i = 0; ; i++) {
        //__asm __volatile("hlt");
        sysyield();
    }    
}

void testTraverseQueue(pcb* queueHead) {
    while(queueHead != NULL) {
        printPcbData(queueHead);
        queueHead = queueHead->next;
    }
}

void printPcbData(pcb* proc) {
    kprintf("proc %d, next %d, prev %d senderHead %d\n", 
        proc, proc->next, proc->prev, proc->senderQueue);
}