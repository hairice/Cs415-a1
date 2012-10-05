/* disp.c : dispatcher
 */

#include <xeroskernel.h>

void initializeProcessQueue();
void dispatch();
void syscreate();
void sysstop();
void sysyield();
void ready(struct pcb*);
void create();
struct pcb* next();
void printPcbData(char*, struct processQueueNode*);


/* Your code goes here */

static struct pcb* currentProcess;
static struct processQueueNode* readyQueueBeginning;
static struct processQueueNode* pcbTable[0];


struct pcb {
	int pid;
	int state;
	int priority;
};

struct processQueueNode {
	struct processQueueNode* prev;
	struct processQueueNode* next;
	struct pcb* process;
};


/**
	Initializes the process ready queue
*/
void initializeProcessQueue() { 
	struct pcb* process;
	process->state = READY;
	readyQueueBeginning->process = process;
	readyQueueBeginning->next = readyQueueBeginning;
	readyQueueBeginning->prev = readyQueueBeginning;
	pcbTable[0] = readyQueueBeginning;
}


void dispatch() { 
	struct pcb* process = next();
	for( ;; ) {
		//short request = contextswitch( process );
		short request = CREATE;
		switch(request) {
			case( CREATE ): create(); break;
			//case( YIELD ): ready( process ); process = next(); break;
			//case( STOP ): cleanup( process ); process = next(); break;
		}
	}
}

/**
	Removes the next process from the ready queue and 
	returns an index or a pointer to its process control block
*/
struct pcb* next() {
	struct pcb* process = readyQueueBeginning->process;

	kprintf("\nreadyQueueBeginning1: %d; readyQueueBeginning next: %d\n", readyQueueBeginning, readyQueueBeginning->next);
	
	readyQueueBeginning = readyQueueBeginning->next;
	return process;
}

void create() {
	kprintf("\nCreating new process\n");
	struct pcb* process;
	ready(process);
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
void ready(struct pcb* process) {
	struct processQueueNode* finalQueueNode = readyQueueBeginning->prev;
	struct processQueueNode* newQueueNode;
	
	process->state = READY;
	newQueueNode->process = process;

	newQueueNode->next = readyQueueBeginning;
	newQueueNode->prev = finalQueueNode;
	finalQueueNode->next = newQueueNode;
	readyQueueBeginning->prev = newQueueNode;
		
	printPcbData("final", finalQueueNode);
	printPcbData("queue", readyQueueBeginning);
	printPcbData("new", newQueueNode);
}


void printPcbData(char* info, struct processQueueNode* node) {
	kprintf("%s pcb: %d, ->prev: %d, ->next: %d, ->state: %d\n", info,
		node, node->prev, node->next, node->process->state);
}
