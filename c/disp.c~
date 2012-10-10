/* disp.c : dispatcher
 */

#include <xeroskernel.h>

void initializeQueue(int index, struct pcb* queue);
void initializePcbTable();
void initializeFreeQueue();
void initializeReadyQueue();
void dispatch();
void ready(struct pcb*);
struct pcb* next();
void cleanup(struct pcb*);
extern struct pcb* getFreeProcess();
extern struct pcb* getPcbByPid(int pid);
void printPcbData(char*, struct pcb*);
void insertNodeAtEndOfQueue(struct pcb* node, struct pcb* queueBeginning);
extern void root(void);

/* Your code goes here */

static struct pcb* readyQueue;
static struct pcb* freeQueue;
static struct pcb* pcbTable[31];


extern void initializeProcesses() {
	initializeReadyQueue();
	initializeFreeQueue();
	
	printPcbData("\nfree", freeQueue);
	
	initializePcbTable();
}

void initializeReadyQueue() {
	readyQueue->next = 0;
	readyQueue->prev = 0;
	readyQueue = 0;
}

void initializeFreeQueue() {
	struct pcb* pcb = (struct pcb*) kmalloc(sizeof(struct pcb));
	pcb->next = pcb;
	pcb->prev = pcb;
	pcb->pid = 0;
	freeQueue = pcb;
	pcbTable[0] = pcb;
}

void initializePcbTable() {
	int i;
	for (i = 1; i < 32; i++) {
		pcbTable[i] = (struct pcb*) kmalloc(sizeof(struct pcb));
		pcbTable[i]->pid = i;
		insertNodeAtEndOfQueue(pcbTable[i], freeQueue);
	}
}

void dispatch() {
	kprintf("d");
	struct pcb* process = next();
	kprintf("Got the next process\n");
	for( ;; ) {
		printContext("\ndispatch", process->context);
		int request;
		request = contextswitch(process);
		switch(request) {
			case(CREATE): {
				kprintf("Creating\n"); create(process->context->eip, 256);
				break;
			}
			case(YIELD): kprintf("yielding\n"); ready(process); process = next(); break;
			case(STOP): kprintf("Stopping\n"); cleanup(process); process = next(); break;
		}
		kprintf("D");
	}
}

/**
	Removes the next process from the ready queue and 
	returns an index or a pointer to its process control block
*/
struct pcb* next() {
	traverseQueue("Next ready", readyQueue);
	struct pcb* process = readyQueue;
	
	if (!process) {
		kprintf("There were no ready processes :(\n");
		return 0;
	}

	readyQueue = readyQueue->next;
	process->prev = 0;
	process->next = 0;

	return process;
}

/**
	Takes an index or pointer to a process control block and 
	adds it to the ready queue
*/
void ready(struct pcb* process) {	
	if (!(int) readyQueue) {
		kprintf("Nothing in rdy queue right now\n");		
		readyQueue = process;
		process->next = process;
		process->prev = process;
	} else {
		insertNodeAtEndOfQueue(process, readyQueue);
	}

	traverseQueue("ready Queue", readyQueue);
	printPcbData("ready queue", readyQueue);
}

void cleanup(struct pcb* process) {
	kfree(process->context);
	insertNodeAtEndOfQueue(process, freeQueue);
}

extern struct pcb* getFreeProcess() {
	//traverseQueue("free queue", freeQueue);
	struct pcb* freeNode = freeQueue;
	freeQueue = freeQueue->next;
	if (freeNode) {
		return freeNode;
	} else {
		return 0;
	}
}

extern struct pcb* getPcbByPid(int pid) {
	return pcbTable[pid];
}

void insertNodeAtEndOfQueue(struct pcb* node, struct pcb* queueBeginning) {
	if (!(int) queueBeginning) {
		node->next = node;
		node->prev = node;
		queueBeginning = node->next;
	} else {
		struct pcb* finalQueueNode = queueBeginning->prev;
		node->next = queueBeginning;
		node->prev = finalQueueNode;
		finalQueueNode->next = node;
		queueBeginning->prev = node;
	}
}

void printPcbData(char* info, struct pcb* node) {
	kprintf("%s pcb: %d, ->prev: %d, ->next: %d\n", info,
		node, node->prev, node->next);
}

void traverseQueue(char* msg, struct pcb* node) {
	kprintf("%s: ", msg);
	int i = 0;

	struct pcb* currNode = node;
	while ((i == 0 || currNode != node) && i < 13) {
		printPcbData(kprintf("%d", i), currNode);
		i++;
		currNode = currNode->next;
	}

	for (i=0; i < 10000000; i++) ;
}
