/* syscall.c : syscalls
 */

#include <xeroskernel.h>
#include <stdarg.h>


int syscall( int req, ... ) {
/**********************************/

    va_list     ap;
    int         rc;

    va_start( ap, req );

    __asm __volatile( " \
        movl %1, %%eax \n\
        movl %2, %%edx \n\
        int  %3 \n\
        movl %%eax, %0 \n\
        "
        : "=g" (rc)
        : "g" (req), "g" (ap), "i" (KERNEL_INT)
        : "%eax" 
    );
 
    va_end( ap );

    return( rc );
}

 int syscreate( funcptr fp, int stack ) {
/*********************************************/

    return( syscall( SYS_CREATE, fp, stack ) );
}

 int sysyield( void ) {
/***************************/

    return( syscall( SYS_YIELD ) );
}

 int sysstop( void ) {
/**************************/

    return( syscall( SYS_STOP ) );
}

int sysgetpid( void ) {
/****************************/

    return( syscall( SYS_GETPID ) );
}

void sysputs( char *str ) {
/********************************/

    syscall( SYS_PUTS, str );
}

unsigned int syssleep( unsigned int t ) {
/*****************************/

    return syscall( SYS_SLEEP, t );
}


/**
 * Registers the provided function as the handler for the indicated signal. 
 * 0 is returned on success
 * If signal is invalid then −1 is returned. 
 * If it can be determined that the handler resides at an invalid address then -2
 * is returned. The function being registered as the handler takes a single argument.
 * The third argument, oldHandler, is a pointer to a variable that points to a 
 * handler. When this call installs a new handler it copies the address
 * of the older handler to the location pointed to by oldHandler
 * 
 */
int syssighandler(int signalNumber, void (*newhandler)(void *), void (**oldHandler)(void *)) {
    return syscall(SYS_SIGHANDLER, signalNumber, newhandler, oldHandler);

    /*      // TEST CODE
    kprintf("Calling sighandler with signal %d and newhandler %d\n", signal, newhandler);
    int ret = syscall(SYS_SIGHANDLER, signal, newhandler, oldHandler);
    
    // Testing old handler
    kprintf("The old handler is %d; calling oldHandler function if not 0\n", *oldHandler);
    funcptr oldHandlerFunc = (funcptr)(*oldHandler);
    if (*oldHandler != 0) oldHandlerFunc();
    
    kprintf("sighandler return value: %d\n", ret);
    return ret;
    */
}

/**
 * To be used only by the trampoline code. Replaces the stored stack pointer for
 * the process with old_sp. Adjusts the mask that indicates what signals are being
 * accepted, and recovers any return code pushed onto the stack.
 * 
 * @param old_sp  the location in the application stack of the context frame to switch
 *      the process to
 */
void sigreturn(void *old_sp) {
    kprintf("In sigreturn() - old_sp: %d\n", old_sp);
    return syscall(SYS_SIGRETURN, old_sp);
}

/**
 * Requests that a signal be delivered to a process.
 * @param PID  the pid of the process to deliver the signal to.
 * @param signalNumber  the number of the signal to be delivered (0 - 31)
 * @return  0 on success.
 *      -18 if the target process doesn't exist
 *      -3 if the signal number is invalid
 */
int syskill(int PID, int signalNumber) {
    //return syscall(SYS_KILL, PID, signalNumber);
    int ret = syscall(SYS_KILL, PID, signalNumber);
    return ret;
}

/**
 * Causes the current process to be suspended until a signal is delivered to it.
 * @return 
 */
int syssigwait(void) {
    //return syscall(SYS_SIGWAIT);
    int ret = syscall(SYS_SIGWAIT);
    sprintf("sigwait: %d\n", ret);
    return ret;
}

/**
 * Opens a device.
 * @param device_no  the major device number - can be used to index into the
 *   device table
 * @return an integer representing a file descriptor.
 *      Ranges from 0-3 on success; -1 on failure
 */
extern int sysopen(int device_no) {
    return syscall(SYS_OPEN, device_no);
}

/**
 * Closes the provided descriptor
 * @param fd  the file descriptor from a previous open call
 * @return 0 on success; -1 on failure
 */
extern int sysclose(int fd) {
    return syscall(SYS_CLOSE, fd);
}

/**
 * Performs  write operation to the device associated with the provided file descriptor
 * Up to bufflen bytes are written from buff
 * @param fd  the file descriptor of the file to write to.
 * @param buff  The data to write
 * @param bufflen  The amount of data to write in bytes
 * @return  the number of bytes written; -1 on failure
 */
extern int syswrite(int fd, void* buff, int bufflen) {
    return syscall(SYS_WRITE, fd, buff, bufflen);
}

/**
 * Reads up to bufflen bytes from the given file descriptor's device and stores
 * the read data in the buffer area pointed to by buff
 * @param fd  the file descriptor associated with the file to read from
 * @param buff  location where data read is stored
 * @param bufflen  the number of bytes to read
 * @return  The number of bytes read or -1 on failure
 */
extern int sysread(int fd, void* buff, int bufflen) {
    return syscall(fd, buff, bufflen);
}

/**
 * Executes the specified control command. The action taken is device specific
 * and depends upon the control command
 * @param fd  file descriptor
 * @param command  the device specific command to perform
 * @param ...  additional parameters are device specific
 * @return  0 on success; -1 on failure
 */
extern int sysioctl(int fd, unsigned long command, ...) {
    return syscall(fd, command);
}

void testHandler() {
    kprintf("Hello, I am the replacement handler\n");
}

void testOldHandler() {
    kprintf("I am the first handler\n");
}