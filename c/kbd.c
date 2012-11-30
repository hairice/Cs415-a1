/* kbc.c : Keyboard
 */

#include <xeroskernel.h>
#include <stdarg.h>

/**
 * Not supported by keyboards
 * @return  an error indication of -1
 */
extern int kbdwrite() {
    return -1;
}

extern int kbdread(char typed) {
    // kprintf or kputc?
    kprintf("%s", typed);
    return 0;
}