/* disp.c : dispatcher
 */

#include <xeroskernel.h>
#include <i386.h>
#include <xeroslib.h>
#include <stdarg.h>


static pcb *head = NULL;
static pcb *tail = NULL;

void dispatch(void) {
    /********************************/

    pcb *p;
    int r;
    funcptr fp;
    int stack;
    va_list ap;
    char *str;
    int len;

    for (p = next(); p;) {
        //      kprintf("Process %x selected stck %x\n", p, p->esp);

        r = contextswitch(p);
        switch (r) {
            case( SYS_CREATE):
                ap = (va_list) p->args;
                fp = (funcptr) (va_arg(ap, int));
                stack = va_arg(ap, int);
                p->ret = create(fp, stack);
                break;
            case( SYS_YIELD):
                ready(p);
                p = next();
                break;
            case( SYS_STOP):
                p->state = STATE_STOPPED;
                p = next();
                break;
            case( SYS_PUTS):
                ap = (va_list) p->args;
                str = va_arg(ap, char *);
                kprintf("%s", str);
                p->ret = 0;
                break;
            case( SYS_GETPID):
                p->ret = p->pid;
                break;
            case( SYS_SLEEP):
                ap = (va_list) p->args;
                len = va_arg(ap, int);
                sleep(p, len);
                p = next();
                break;
            case(SYS_SIGHANDLER):
                ap = (va_list) p->args;
                int signal = va_arg(ap, int);
                funcptr newHandler = (funcptr) (va_arg(ap, int));
                funcptr* oldHandler = (funcptr) (va_arg(ap, int*));
                
                if (!isValidSignalNumber(signal)) {
                    p->ret = -1;
                    break;
                } else if (!isValidHandlerAddress((unsigned int) newHandler)) {
                    p->ret = -2;
                    break;
                }

                // DOES THIS WORK?
                signalEntry* pcbSignalEntry = &p->signalTable[signal];
                *oldHandler = pcbSignalEntry->handler;
                pcbSignalEntry->handler = newHandler;

                p->ret = 0;
                break;
            case(SYS_SIGWAIT):
                p->state = STATE_STOPPED;
                p = next();
                break;
            case(SYS_SIGRETURN):
                ap = (va_list) p->args;
                void* old_sp = va_arg(ap, void*);
                p->esp = old_sp;
                
                // recover other data...
                
                break;
            case(SYS_KILL):
                ap = (va_list) p->args;
                int PID = va_arg(ap, int);
                int signalNumber = va_arg(ap, int);
                pcb* receivingPcb = findPCB(PID);
                
                if (!isValidSignalNumber(signalNumber)) { 
                    p->ret = -1;
                    break;
                } else if (!receivingPcb) { 
                    p->ret = -18; 
                    break;
                }
                
                // KILL

                p->ret = 0;
                break;
            case( SYS_TIMER):
                tick();
                ready(p);
                p = next();
                end_of_intr();
                break;
            default:
                kprintf("Bad Sys request %d, pid = %d\n", r, p->pid);
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

    if (tail) {
        tail->next = p;
    } else {
        head = p;
    }

    tail = p;
}

extern pcb *next(void) {
    /*****************************/

    pcb *p;

    p = head;

    if (p) {
        head = p->next;
        if (!head) {
            tail = NULL;
        }
    } else {
        kprintf("BAD\n");
        for (;;);
    }

    p->next = NULL;
    p->prev = NULL;
    return ( p);
}

extern pcb *findPCB(int pid) {
    /******************************/

    int i;

    for (i = 0; i < MAX_PROC; i++) {
        if (proctab[i].pid == pid) {
            return ( &proctab[i]);
        }
    }

    return ( NULL);
}

extern int isValidHandlerAddress(unsigned int handlerAddress) {
    return 1;
}

extern int isValidSignalNumber(int signal) {
    if (signal > 31 || signal < 0) return 0;
    else return 1;
}