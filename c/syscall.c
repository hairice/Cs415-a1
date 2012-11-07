/* syscall.c : syscalls
 */

#include <xeroskernel.h>
#include <stdarg.h>

int syscall(int req, ...) {
    /**********************************/

    va_list argList;
    unsigned int rc;

    va_start(argList, req);

    __asm __volatile(" \
        movl %1, %%eax \n\
        movl %2, %%edx \n\
        int  %3 \n\
        movl %%eax, %0 \n\
        "
            : "=g" (rc)
            : "g" (req), "g" (argList), "i" (KERNEL_INT)
            : "%eax"
            );

    va_end(argList);

    return ( rc);
}


/**
        Takes a function pointer denoting the address to start execution at
        and an integer that denotes the size of the new process’s stack. The
        function returns the process ID of the created process.
 */
int syscreate(funcptr fp, int stack) {
    /*********************************************/

    return ( syscall(SYS_CREATE, fp, stack));
}

int sysyield(void) {
    /***************************/

    return ( syscall(SYS_YIELD));
}

int sysstop(void) {
    /**************************/

    return ( syscall(SYS_STOP));
}

extern int systimerint() {
    return (syscall(TIMER_INT));
}

/**
 * Returns the pid of the current process
 */
extern unsigned int sysgetpid() {
    return getCurrentPid();
}

/**
 * Used by processes to perform output
 * @param char* str  null terminated string that is to be displayed by the kernel
 */
extern void sysputs(char *str) {
    kprintf(str);
}

extern unsigned int getSysStopAddr() {
    return &sysstop;
}

/**
 *  returns the number of bytes accepted by the receiving process or a negative
 *  value if the operation failed. The system call blocks until the send operation
 *  completes. The call returns −1 if the destination process does not exist, 
 *      −2 if the process is trying to send to itself, and 
 *      −3 for any other sending error
 * 
 * @param dest_pid
 * @param buffer - address of source buffer
 * @param buffer_len - buffer's length in bytes
 * @return - number of bytes successfully sent
 */
extern int syssend(unsigned int dest_pid, void *buffer, int buffer_len) {
    //kprintf("in syssend - buffer addr: %d\n", &buffer);
    if (dest_pid <= 0) return -1;
    if (getCurrentPid() == dest_pid) return -2;

    pcb* sendingProc = getProcessByPid(getCurrentPid());

    // if (!sendingProc) return -1;

    //int sentBytes = syscall(SYS_SEND, dest_pid, buffer, buffer_len, sendingProc);
    return syscall(SYS_SEND, dest_pid, buffer, buffer_len, sendingProc);
    //return sentBytes;
}

/**
 * Sets the process up to receive a message. If no messages are available, the
 * process is blocked until one is sent to it.
 * 
 * Returns the number of bytes that were received OR
 *      -1 if the PID to receive from is invalid
 *      -2 if the process tries to receive from itself
 *      -3 for any other problem (eg. size is negative)
 * @param from_pid - contains the address of the pid the process is receiving from
 * @param buffer - address of the destination buffer
 * @param buffer_len - length of the buffer
 * @return number of bytes successfully received
 */
extern int sysrecv(unsigned int *from_pid, void *buffer, int buffer_len) {
    //kprintf("in sysrecv\n");
    if (buffer_len < 0) {
        kprintf("Negative buffer length\n");
        return -3;
    }

    if (from_pid == getCurrentPid()) {
        kprintf("from pid = current pid\n");
        return -2;
    }

    return (syscall(SYS_RECEIVE, from_pid, buffer, buffer_len));
}

/**
 * Puts the current process to sleep for a given minimum number of milliseconds
 * @return 0 if the process slept for the planned amount of time, otherwise
 *     return the amount of time that the process still had to sleep
 */
extern unsigned int syssleep(unsigned int milliseconds) {
    int ticks = milliseconds / 10;
    return syscall(SYS_SLEEP, ticks);
}