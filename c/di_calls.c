/* di_calls.c : Dispatch Calls
 */

#include <xeroskernel.h>
#include <stdarg.h>

int isFdValid(int fd);

devsw   deviceTable[4];

extern void deviceinit() {
    devsw* keyboardNonEcho = &deviceTable[KBD_NONECHO];
    keyboardNonEcho->dvwrite = (funcptr)(kbdwrite);
    keyboardNonEcho->dvread = (funcptr)(kbdNonEchoRead);
    keyboardNonEcho->dvopen = (funcptr)(kbdopen);
    keyboardNonEcho->dvclose = (funcptr)(kbdclose);
    keyboardNonEcho->dvcntl = (funcptr)(kbdioctl);
    
    devsw* keyboardEcho = &deviceTable[KBD_ECHO];
    keyboardEcho->dvwrite = (funcptr)(kbdwrite);
    keyboardEcho->dvread = (funcptr)(kbdEchoRead);
    keyboardEcho->dvopen = (funcptr)(kbdopen);
    keyboardEcho->dvclose = (funcptr)(kbdclose);
    keyboardEcho->dvcntl = (funcptr)(kbdioctl);
}

/**
 * Opens a device. @see sysopen()
 * @param device_no
 * @return 
 */
extern int di_open(pcb* proc, int device_no) {
    kprintf("In device_open()\n");
    if (device_no < 0 || device_no > 3) return -3;    
    
    
    int ret = -1;
    int majorNum = proc->fileDescriptorTable[device_no].majorNum;
    devsw* device = &deviceTable[majorNum];
    funcptr openHandler = (funcptr)(device->dvopen);
    if (openHandler) ret = device->dvopen(proc, device, device_no);

    
    kprintf("Leaving di_open()\n");
    return ret;
}

/**
 * Closes a device. @see sysclose()
 * @param fd
 * @return 
 */
extern int di_close(pcb* proc, int fd) {
    if (!isFdValid(fd)) return -1;
    
    // Check to see if device is open
    // Mark as inactive in FDT
    
    if (proc->fileDescriptorTable[fd].device) {
        devsw* device = proc->fileDescriptorTable[fd].device;
        funcptr closeHandler = (funcptr)(device->dvclose);
        if (closeHandler) closeHandler( );
    }
    
    return 0;
}

/**
 * Writes bufflen bytes to buffer buff. @see syswrite()
 * @param fd
 * @param buff
 * @param bufflen
 * @return 
 */
extern int di_write(int fd, void* buff, int bufflen) {
    if (!isFdValid(fd)) return -1;
    
    return deviceTable[fd].dvwrite(buff, bufflen);
}

extern int di_read(int fd, void* buff, int bufflen) {
    if (!isFdValid(fd)) return -1;
    kprintf("In di_read()\n");    
    return deviceTable[fd].dvread(buff, bufflen);
}

extern int di_ioctl(int fd, unsigned long command, ...) {
    if (!isFdValid(fd)) return -1;
    
    return deviceTable[fd].dvcntl(command);
}

int isFdValid(int fd) {
    return (fd >=0 && fd <=3);   
}