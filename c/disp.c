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
    int signalNumber;
    int fd;
    void* buff;
    int bufflen;
    

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
                signalNumber = va_arg(ap, int);
                funcptr newHandler = (funcptr) (va_arg(ap, int));
                funcptr* oldHandler = (funcptr) (va_arg(ap, int*));
                
                if (!isValidSignalNumber(signalNumber)) {
                    p->ret = -1;
                    break;
                } else if (!isValidHandlerAddress((unsigned int) newHandler)) {
                    p->ret = -2;
                    break;
                }
                
                signalEntry* pcbSignalEntry = &p->signalTable[signalNumber];
                
                *oldHandler = (funcptr)(pcbSignalEntry->handler);
                //kprintf("pcb->handler: %d, oldHandler: %d, *oldHandler: %d\n", pcbSignalEntry->handler, oldHandler, *oldHandler);
                
                pcbSignalEntry->handler = newHandler;
                
                // test new handler
                //kprintf("testing new handler\n");
                //pcbSignalEntry->handler();
                

                p->ret = 0;
                break;
            case(SYS_SIGWAIT):
                p->state = STATE_SIGWAIT;
                p = next();
                break;
            case(SYS_KILL):
                ap = (va_list) p->args;
                int pid = va_arg(ap, int);
                signalNumber = va_arg(ap, int);
                pcb* receivingPcb = findPCB(pid);
                
                if (!isValidSignalNumber(signalNumber)) { 
                    p->ret = -3;
                    break;
                } else if (!receivingPcb) { 
                    p->ret = -18; 
                    break;
                }
                
                kprintf("going into signal\n");
                p->ret = signal(pid, signalNumber);
                kprintf("Returning from signal. Return val: %d\n", p->ret);
                
                if (receivingPcb->state == STATE_SIGWAIT) ready(receivingPcb);

                break;
            case(SYS_SIGRETURN):
                kprintf("in the dispatch part of sigreturn()...\n");    
                ap = (va_list) p->args;
                long old_sp = (long) va_arg(ap, void*);
                
                kprintf("curr_sp: %d, old_sp: %d\n", p->esp, old_sp);
                
                p->esp = (long) old_sp;
                
                // TODO: recover other data...
                signal_stack* signalStack = (signal_stack*)(p->esp - sizeof(signal_stack));
                kprintf("signalStack handler %d, esp %d, ret %d\n",
                    signalStack->handler, signalStack->esp, signalStack->ret);
                
                p->ret = 0;
                
                break;
                
            case(SYS_OPEN):
                ap = (va_list) p->args;
                int device_no = va_arg(ap, int);
                p->ret = di_open(device_no);
                break;
            case(SYS_CLOSE):
                ap = (va_list) p->args;
                fd = va_arg(ap, int);
                p->ret = di_close(fd);
                break;
            case(SYS_WRITE):
                ap = (va_list) p->args;
                fd = va_arg(ap, int);
                buff = va_arg(ap, void*);
                bufflen = va_arg(ap, int);
                
                p->ret = di_write(fd, buff, bufflen);
                break;
            case(SYS_READ):
                ap = (va_list) p->args;
                fd = va_arg(ap, int);
                buff = va_arg(ap, void*);
                bufflen = va_arg(ap, int);
                p->ret = di_read(fd, buff, bufflen);
                break;
            case(SYS_IOCTL):
                ap = (va_list) p->args;
                fd = va_arg(ap, int);
                unsigned long command = va_arg(ap, unsigned long);
                
                p->ret = di_ioctl(fd, command);
                break;
            case(SYS_TIMER):
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

extern int isValidSignalNumber(int signalNumber) {
    if (signalNumber > 31 || signalNumber < 0) return 0;
    else return 1;
}