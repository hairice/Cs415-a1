/* signal.c : signal things
 */

#include <xeroskernel.h>
#include <stdarg.h>

void sigtramp(void (*handler)(void *), void *contextFrame, void *old_sp) {
    kprintf("in sigtramp()\n");
    kprintf("Handler: %d\n", handler);
    
    kprintf("old_sp: %d\n", old_sp);
    
    context_frame* cFrame = (context_frame*) contextFrame;
    kprintf("context frame: %d; ebp: %d, esp: %d, eip: %d\n", 
            cFrame, cFrame->ebp, cFrame->esp, cFrame->iret_eip);
    
/*
    int i;
    for(i = 0; i < 100000000; i++);
*/
    handler(cFrame);
    sigreturn(old_sp);
    /* Control never gets back to here */
}

/**
 * Called when a signal is to be delivered to a process.
 * @param pid  the pid of the process to deliver the signal to
 * @param sig_no  the signal to deliver
 * @return 0 on success, -1 if PID is invalid, -2 if signal is invalid
 */
extern int signal(int pid, int sig_no) {
    kprintf("In signal()\n");
    if (!isValidSignalNumber(sig_no)) return -2;
    
    pcb* p = findPCB(pid);
    
    if (!p) return -1;
        
    int signalsWaiting = p->signalsWaiting;
    
    signalEntry* signal_entry = &p->signalTable[sig_no];
    funcptr handler = (funcptr)(signal_entry->handler);
    long old_sp = p->esp;
    
    
    unsigned int stackPos = p->esp;
    stackPos -= sizeof(signal_stack);
    signal_stack* signalStack = (signal_stack*)(stackPos);
    signalStack->handler = handler;
    signalStack->esp = old_sp;
    signalStack->ret = p->ret;
    signalStack->old_sp = old_sp;

        
    stackPos -= sizeof(context_frame);
    context_frame* contextFrame = (context_frame*)(stackPos);
    contextFrame->ebp = stackPos;
    contextFrame->esp = stackPos;
    contextFrame->iret_eip = &sigtramp;
    contextFrame->iret_cs = getCS();
    contextFrame->eflags = 0x00003200;    
        
    p->esp = stackPos;
    
    kprintf("sstack esp: %d, hand %d, old %d, ret %d\n",
        signalStack->esp, signalStack->handler, signalStack->old_sp, signalStack->ret);
    
    kprintf("Handler: %d\n", handler);
    kprintf("context frame: %d; ebp: %d, esp: %d, eip: %d\n", 
            contextFrame, contextFrame->ebp, contextFrame->esp, contextFrame->iret_eip);
    
    
    /*
    if (!signalsWaiting) {
        kprintf("No signals waiting\n");
        // Nothing waiting
        context_frame* contextFrame = kmalloc(256);
        signalEntry* signal_entry = &p->signalTable[sig_no];
        funcptr handler = (funcptr)(signal_entry->handler);
        long old_sp = p->esp + 24;
        sigtramp(handler, contextFrame, old_sp);
        kprintf("called sigtramp...\n");
    } else if (signalsWaiting >= sig_no) {
        // place on wait list
    } else {
        // interrupt other signals
    }
    */
    
    return 0;
}

int isSignalWaiting(pcb* p, int signalNumber) { 
    int signalsWaiting = p->signalsWaiting;
    return signalsWaiting & (1 << signalNumber);
}