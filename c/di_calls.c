/* di_calls.c : Dispatch Calls
 */

#include <xeroskernel.h>
#include <stdarg.h>

int isFdValid(int fd);

devsw   deviceTable[4];

extern void deviceinit() {
    devsw* keyboard1 = &deviceTable[0];
    keyboard1->dvwrite = (funcptr)(kbdwrite);
    keyboard1->dvread = (funcptr)(kbdread);
}

/**
 * Opens a device. @see sysopen()
 * @param device_no
 * @return 
 */
extern int di_open(int device_no) {
    if (device_no < 0 || device_no > 3) return -3;    
    
    // TODO: Check to see if the device is already open
    
    pcb* process = findPCB(sysgetpid());    
    
    if (!process->fileDescriptorTable[device_no]->status) {
        int majorNum = process->fileDescriptorTable[device_no];
        devsw* device = &deviceTable[majorNum];
        funcptr openHandler = (funcptr)(device->dvopen);
        if (openHandler) openHandler();
        
        process->fileDescriptorTable[device_no]->status = 1;
        process->fileDescriptorTable[device_no]->device = device;
    }
    
    return device_no;
}

/**
 * Closes a device. @see sysclose()
 * @param fd
 * @return 
 */
extern int di_close(int fd) {
    if (!isFdValid(fd)) return -1;
    
    // Check to see if device is open
    // Mark as inactive in FDT
    
    pcb* process = findPCB(sysgetpid());
    if (process->fileDescriptorTable[fd]->status) {
        process->fileDescriptorTable[fd]->status = 0;
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
    
    return deviceTable[fd].dvread(buff, bufflen);
}

extern int di_ioctl(int fd, unsigned long command, ...) {
    if (!isFdValid(fd)) return -1;
    
    return deviceTable[fd].dvseek(command);
}

int isFdValid(int fd) {
    return (fd >=0 && fd <=3);   
}