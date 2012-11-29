/* signal.c : signal things
 */

#include <xeroskernel.h>
#include <stdarg.h>

void sigtramp(void (*handler)(void *), void *contextFrame, void *old_sp) {
    handler(contextFrame);
    sigreturn(old_sp);
    /* Control never gets back to here */
}

/**
 * Called when a signal is to be delivered to a process.
 * @param pid  the pid of the process to deliver the signal to
 * @param sig_no  the signal to deliver
 * @return 0 on success, -1 if PID is invalid, -2 if signal is invalid
 */
int signal(int pid, int sig_no) {
    if (!isValidSignalNumber(sig_no)) return -2;
    
    pcb* p = findPCB(pid);
    
    if (!p) return -1;
    
    
    
    return 0;
}
