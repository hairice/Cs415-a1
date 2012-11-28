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
int syssighandler(int signal, void (*newhandler)(void *), void (**oldHandler)(void *)) {
    if (signal > 31 || signal < 0) return -1;
}

void sigreturn(void *old_sp) {
    
}

int syskill(int PID, int signalNumber) {
    
}

int syssigwait(void) {
    
}