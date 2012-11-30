/* xeroskernel.h - disable, enable, halt, restore, isodd, min, max */

/* Symbolic constants used throughout Xinu */

typedef	char		Bool;		/* Boolean type			*/
#define	FALSE		0		/* Boolean constants		*/
#define	TRUE		1
#define	EMPTY		(-1)		/* an illegal gpq		*/
#define	NULL		0		/* Null pointer for linked lists*/
#define	NULLCH		'\0'		/* The null character		*/


/* Universal return constants */

#define	OK		 1		/* system call ok		*/
#define	SYSERR		-1		/* system call failed		*/
#define	EOF		-2		/* End-of-file (usu. from read)	*/
#define	TIMEOUT		-3		/* time out  (usu. recvtim)	*/
#define	INTRMSG		-4		/* keyboard "intr" key pressed	*/
					/*  (usu. defined as ^B)	*/
#define	BLOCKERR	-5		/* non-blocking op would block	*/

/* Functions defined by startup code */


void bzero(void *base, int cnt);
void bcopy(const void *src, void *dest, unsigned int n);
int kprintf(char * fmt, ...);
void lidt(void);
void init8259(void);
void disable(void);
void outb(unsigned int, unsigned char);
unsigned char inb(unsigned int);

extern void kmeminit(void);
extern void *kmalloc(int size);
extern void kfree(void *ptr);

#define MAX_PROC        64
#define KERNEL_INT      80
#define TIMER_INT       (TIMER_IRQ + 32)
#define PROC_STACK      (4096 * 4)

#define STATE_STOPPED   0
#define STATE_READY     1
#define STATE_SLEEP     2
#define STATE_SIGWAIT   3

#define SYS_STOP        0
#define SYS_YIELD       1
#define SYS_CREATE      2
#define SYS_TIMER       3
#define SYS_GETPID      8
#define SYS_PUTS        9
#define SYS_SLEEP       10
#define SYS_SIGWAIT     11
#define SYS_SIGRETURN   12
#define SYS_KILL        13
#define SYS_SIGHANDLER  14
#define SYS_OPEN        15
#define SYS_CLOSE       16
#define SYS_WRITE       17
#define SYS_READ        18
#define SYS_IOCTL       19

typedef struct devsw {
    int dvnum;
    char *dvname;
    int (*dvinit)();
    int (*dvopen)();
    int (*dvclose)();
    int (*dvread)();
    int (*dvwrite)();
    int (*dvseek)();
    int (*dvgetc)();
    int (*dvputc)();
    int (*dvcntl)();
    void *dvcsr;
    void *dvivec;
    void *dvovec;
    int (*dviint)();
    int (*dvoint)();
    void *dvioblk;
    int dvminor;
} devsw;

extern devsw   deviceTable[4];


typedef void    (*funcptr)(void);

typedef struct signalEntry {
    funcptr handler;
} signalEntry;

typedef struct struct_pcb pcb;
struct struct_pcb {
  long        esp;
  pcb         *next;
  pcb         *prev;
  int         state;
  int         pid;
  int         otherpid;
  signalEntry signalTable[32];
  int         signalsWaiting;
  int         fileDescriptorTable[4];
  void*       buffer;
  int         bufferlen;
  int         ret;
  int         sleepdiff;
  long        args;

};

extern pcb     proctab[MAX_PROC];
#pragma pack(1)

typedef struct context_frame {
  unsigned int        edi;
  unsigned int        esi;
  unsigned int        ebp;
  unsigned int        esp;
  unsigned int        ebx;
  unsigned int        edx;
  unsigned int        ecx;
  unsigned int        eax;
  unsigned int        iret_eip;
  unsigned int        iret_cs;
  unsigned int        eflags;
  unsigned int        stackSlots[0];
} context_frame;

typedef struct signal_stack {
    unsigned int ret;
    funcptr handler;
    unsigned int esp;
    unsigned int old_sp;
    int ignoreSignalMask;
}signal_stack;

extern pcb      proctab[MAX_PROC];

unsigned short getCS(void);
void     kmeminit( void );
void     *kmalloc( int size );
extern void deviceinit();
extern int signal(int pid, int sig_no);
void     dispatch( void );
void     dispatchinit( void );
void     ready( pcb *p );
pcb      *next( void );
void     contextinit( void );
int      contextswitch( pcb *p );
int      create( funcptr fp, int stack );
void     set_evec(unsigned int xnum, unsigned long handler);
void     tick(void);
void     sleep(pcb*, int);
void sigtramp(void (*handler)(void *), void *contextFrame, void *old_sp);


extern pcb *findPCB(int pid);
extern int isValidHandlerAddress(unsigned int handlerAddress);
extern int isValidSignalNumber(int signalNumber);

extern void     root( void );
void printCF (void * stack);


/* System Calls - probably should be in a different .h file */

int syscall(int call, ...);
int syscreate( funcptr fp, int stack );
int sysyield(void);
int sysstop(void);
int sysgetpid( void );
void sysputs( char *str );
unsigned int syssleep( unsigned int t );
int syssigwait(void);
int syskill(int PID, int signalNumber);
void sigreturn(void *old_sp);
int syssighandler(int signalNumber, void (*newhandler)(void *), void (**oldHandler)(void *));
extern int sysopen(int device_no);
extern int sysclose(int fd);
extern int syswrite(int fd, void* buff, int bufflen);
extern int sysread(int fd, void* buff, int bufflen);
extern int sysioctl(int fd, unsigned long command, ...);


/* Dispatch calls */
extern int di_open(int device_no);
extern int di_close(int fd);
extern int di_write(int fd, void* buff, int bufflen);
extern int di_read(int fd, void* buff, int bufflen);
extern int di_ioctl(int fd, unsigned long command, ...);



void testHandler();
void testOldHandler();