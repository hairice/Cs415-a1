/* disp.c : dispatcher
 */

#include <xeroskernel.h>

void initializeProcessQueue();
void dispatch();
void syscreate();
void sysstop();
void sysyield();
void ready();
struct pcb* next();


/* Your code goes here */


static struct processQueue* readyQueueBeginning;
static struct processQueue* readyQueueEnd;
static struct processQueue* pcbTable[0];

const short CREATE = 1;
const short YIELD = 2;
const short STOP = 3;


struct processQueue {
	struct readyProcessLink* prev;
	struct readyProcessLink* next;
	struct pcb* process;
};

struct pcb {
	int pid;
	unsigned char state;
	unsigned char priority;
};

/**
	Initializes the process ready queue
*/
void initializeProcessQueue() { 
	struct pcb* process;
	readyQueueBeginning->process = process;
	readyQueueBeginning->next = 0;
	readyQueueBeginning->prev = 0;
	readyQueueEnd = readyQueueBeginning;
	pcbTable[0] = readyQueueBeginning;
}


void dispatch() { /*
	struct pcb* process = next();
	for( ;; ) {
		request = contextswitch( process );
		switch( request ) {
			case( CREATE ): create( ... ); break;
			case( YIELD ): ready( process ); process = next(); break;
			case( STOP ): cleanup( process ); process = next(); break;
		}
	} */
}

void syscreate() {

}

void sysstop() {

}

void sysyield() {

}

/**
	Takes an index or pointer to a process control block and 
	adds it to the ready queue
*/
void ready() {

}

/**
	Removes the next process from the ready queue and 
	returns an index or a pointer to its process control block
*/
struct pcb* next() {
	struct pcb* process = readyQueueBeginning->process;

	if (readyQueueBeginning != readyQueueEnd) {
		readyQueueBeginning = &readyQueueBeginning->next;
	}

	kprintf("\nreadyQueueBeginning1: %d; readyQueueBeginning next: %d\n", readyQueueBeginning, readyQueueBeginning->next);
	kprintf("\nreadyQueueBeginning2: %d\n", readyQueueBeginning);
	readyQueueBeginning = &readyQueueBeginning->next;
	return process;
}
