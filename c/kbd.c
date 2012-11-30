/* kbc.c : Keyboard
 */

#include <xeroskernel.h>
#include <stdarg.h>

short isKeyboardDataReady();
void addCharToBuffer(char character);

int CTL_D = 0x4;
int ENTERKEY = 0xa;
devsw   deviceTable[4];
unsigned char justPressed;
unsigned char keyboardBuffer[4];

unsigned char eofChar;

/**
 * Not supported by keyboards
 * @return  an error indication of -1
 */
extern int kbdwrite() {
    return -1;
}

extern int kbdEchoRead(void* buff, unsigned int bufflen) {
    kprintf("In echo read - %c\n", justPressed);
    memset(buff, justPressed, bufflen);
    
    //*buff += justPressed;
    return 0;
}

extern int kbdNonEchoRead(void* buff, unsigned int bufflen) {
    kprintf("In NonEcho Read\n");
    return 0;
}

extern int kbdopen(pcb* process, devsw* device, int device_no) {
    kprintf("kbdopen()\n");
    
    file_descriptor* echoKbd = &process->fileDescriptorTable[KBD_ECHO];
    file_descriptor* nonEchoKbd = &process->fileDescriptorTable[KBD_NONECHO];
    
    if (echoKbd->device || nonEchoKbd->device) {
        kprintf("A device is already open. Returning error\n");
        return -1;
    } else {
        kprintf("Opening keyboard %d (0 = nonecho, 1 = echo)\n", device_no);
        enable_irq(1,0);
        process->fileDescriptorTable[device_no].device = device;
    }
    
    return 0;
}

extern int kbdclose(pcb* process, devsw* device, int device_no) {
    kprintf("kbdclose()\n");
    kprintf("Closing keyboard %d (0 = nonecho, 1 = echo)\n", device_no);
    enable_irq(1,1);
    process->fileDescriptorTable[device_no].device = 0;
    return 0;
}

extern void kbdioctl(char newEofChar) {
    eofChar = newEofChar;
}

extern int kbd_handler() {
    //kprintf("in kbd handler");
    if(isKeyboardDataReady()) {
        unsigned char fromPort = inb(0x60);
        unsigned char character = kbtoa(fromPort);
        //kprintf("fromPort: %x, char: %x \n", fromPort, character);
        
        if ((int)character == CTL_D) {
            kprintf("BREAK!!\n");
        } else if ((int) character == ENTERKEY) {
            justPressed = '\n';
            addCharToBuffer(justPressed);
            kprintf("%c", justPressed);
        } else if(character) {
            kprintf("%c", character);
            justPressed = character;
            addCharToBuffer(character);
        }
    }
    
    return 0;
}

void addCharToBuffer(char character) {
    int i;
    for (i = 0; i < 4; i++) {
        if (! keyboardBuffer[i]) {
            keyboardBuffer[i] = character;
        }
    }
}

short isKeyboardDataReady() {
    return (inb(0x64) & 0x01);
}