/* disp.c : dispatcher
 */

#include <xeroskernel.h>

void initializeProcessQueue();
void initializeReadyQueue();
void initializeStoppedQueue();
void dispatch();
void syscreate();
void sysstop();
void sysyield();
void ready(struct pcb*);
struct pcb* next();
void cleanup(struct pcb*);
void printPcbData(char*, struct processQueueNode*);


/* Your code goes here */

static struct pcb* currentProcess;
static struct processQueueNode* readyQueueBeginning;
static struct processQueueNode* stoppedQueue;
static struct processQueueNode* pcbTable[0];

struct processQueueNode {
	struct processQueueNode* prev;
	struct processQueueNode* next;
	struct pcb* process;
};


/**
	Initializes the process ready queue
*/
void initializeProcessQueue() { 
	initializeReadyQueue();	
	initializeStoppedQueue();
}

void initializeReadyQueue() {
	struct processQueueNode* queueNode = kmalloc(sizeof(struct processQueueNode));
	struct pcb* process = kmalloc(sizeof(struct pcb));
	
	process->state = READY;
	queueNode->process = process;
	queueNode->next = queueNode;
	queueNode->prev = queueNode;

	readyQueueBeginning = queueNode;
	pcbTable[0] = readyQueueBeginning;
	
	printPcbData("queue", readyQueueBeginning);
}

void initializeStoppedQueue() {
	struct processQueueNode* queueNode = kmalloc(sizeof(struct processQueueNode));
		
	queueNode->next = queueNode;
	queueNode->prev = queueNode;
	queueNode->process = 0;
	
	stoppedQueue = queueNode;
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
	struct pcb* process = readyQueueBeginning->process;
	kprintf("\n------ DISPATCH NEXT ------\n");
	kprintf("\nreadyQueueBeginning1: %d; readyQueueBeginning next: %d\n", readyQueueBeginning, readyQueueBeginning->next);
	
	readyQueueBeginning = readyQueueBeginning->next;
	return process;
}

/**
	Takes an index or pointer to a process control block and 
	adds it to the ready queue
*/
void ready(struct pcb* process) {
	struct processQueueNode* newQueueNode = kmalloc(sizeof(struct processQueueNode));
	
	process->state = READY;
	newQueueNode->process = process;

	printPcbData("queue", readyQueueBeginning);

	insertNodeAtEndOfQueue(newQueueNode, readyQueueBeginning);

	//printPcbData("queue", readyQueueBeginning);
	//printPcbData("new", newQueueNode);
}

void cleanup(struct pcb* process) {
	process->state = STOPPED;
	if (!stoppedQueue->process) {
		stoppedQueue->process = process;
	} else {
		struct processQueueNode* queueNode;
		queueNode->process = process;
		insertNodeAtEndOfQueue(queueNode, stoppedQueue);
	}
}

void syscreate() {
	
}

void sysstop() {

}

void sysyield() {
	
}



void insertNodeAtEndOfQueue(struct processQueueNode* node, struct processQueueNode* queueBeginning) {
	struct processQueueNode* finalQueueNode = queueBeginning->prev;
	node->next = queueBeginning;
	node->prev = finalQueueNode;
	finalQueueNode->next = node;
	queueBeginning->prev = node;
}

void printPcbData(char* info, struct processQueueNode* node) {
	kprintf("%s pcb: %d, ->prev: %d, ->next: %d, ->state: %d\n", info,
		&node, node->prev, node->next, node->process->state);
}
