/* disp.c : dispatcher
 */

#include <xeroskernel.h>

void initializeProcessQueue();
void dispatch();
void syscreate();
void sysstop();
void sysyield();
void ready(struct pcb*);
struct pcb* next();
void cleanup(struct pcb*);
void printPcbData(char*, struct pcb*);
void insertNodeAtEndOfQueue(struct pcb* node, struct pcb* queueBeginning);


/* Your code goes here */

static struct pcb* currentProcess;
static struct pcb* readyQueue;
static struct pcb* freeQueue;
static struct pcb* pcbTable[31];


/**
	Initializes the process ready queue
*/
void initializeProcessQueue() { 
	initializeProcesses();
}

void initializeProcesses() {
	pcbTable[0] = kmalloc(sizeof(struct pcb));
	pcbTable[0]->next = pcbTable[0];
	pcbTable[0]->prev = pcbTable[0];
	freeQueue = pcbTable[0];
	int i;
	for (i = 1; i < 32; i++) {
		pcbTable[i] = kmalloc(sizeof(struct pcb));
		insertNodeAtEndOfQueue(pcbTable[i], freeQueue);
	}
}

void dispatch() { 
	struct pcb* process = next();
	for( ;; ) {
		//short request = contextswitch( process );
		short request = CREATE;
		switch(request) {
			case(CREATE): create(currentProcess, 16); break;
			case(YIELD): ready(process); process = next(); break;
			case(STOP): cleanup(process); process = next(); break;
		}
	}
}

/**
	Removes the next process from the ready queue and 
	returns an index or a pointer to its process control block
*/
struct pcb* next() {
	struct pcb* process = readyQueue;
	kprintf("\n------ DISPATCH NEXT ------\n");
	kprintf("\nreadyQueue1: %d; readyQueue next: %d\n", readyQueue, readyQueue->next);
	
	readyQueue = readyQueue->next;
	return process;
}

/**
	Takes an index or pointer to a process control block and 
	adds it to the ready queue
*/
void ready(struct pcb* process) {
	printPcbData("queue", readyQueue);
	insertNodeAtEndOfQueue(process, readyQueue);

	//printPcbData("queue", readyQueue);
	//printPcbData("new", newQueueNode);
}

void cleanup(struct pcb* process) {
	kfree(process->stack);
	insertNodeAtEndOfQueue(process, freeQueue);
}

void syscreate() {
	
}

void sysstop() {

}

void sysyield() {
	
}



void insertNodeAtEndOfQueue(struct pcb* node, struct pcb* queueBeginning) {
	struct pcb* finalQueueNode = queueBeginning->prev;
	node->next = queueBeginning;
	node->prev = finalQueueNode;
	finalQueueNode->next = node;
	queueBeginning->prev = node;
}

void printPcbData(char* info, struct pcb* node) {
	kprintf("%s pcb: %d, ->prev: %d, ->next: %d, ->stackSize: %d\n", info,
		&node, node->prev, node->next, node->stack->size);
}
