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


/* Your code goes here */

static struct pcb* readyQueue;
static struct pcb* freeQueue;
static struct pcb* pcbTable[31];


extern void initializeProcesses() {
	initializeReadyQueue();
	initializeFreeQueue();
	
	printPcbData("\nfree", freeQueue);
	printPcbData("ready", readyQueue);
	
	initializePcbTable();
}


void initializeReadyQueue() {
	struct pcb* pcb = (struct pcb*) kmalloc(sizeof(struct pcb));
	pcb->next = pcb;
	pcb->prev = pcb;
	pcb->pid = 0;
	pcbTable[0] = pcb;
	readyQueue = pcbTable[0];
	create(pcb, 256);
}

void initializeFreeQueue() {
	struct pcb* pcb = (struct pcb*) kmalloc(sizeof(struct pcb));
	pcb->next = pcb;
	pcb->prev = pcb;
	pcb->pid = 1;
	freeQueue = pcb;
	pcbTable[1] = pcb;
}

void initializePcbTable() {
	int i;
	for (i = 2; i < 32; i++) {
		kprintf("%d", i);		
		pcbTable[i] = (struct pcb*) kmalloc(sizeof(struct pcb));
		kprintf("a");
		pcbTable[i]->pid = i;
		kprintf("p");
		insertNodeAtEndOfQueue(pcbTable[i], freeQueue);
		kprintf("i ");
	}
}

void dispatch() {
	kprintf("d");
	struct pcb* process = next();
	for( ;; ) {
		printContext("\ndispatch", process->context);
		int request;
		request = contextswitch();
		switch(request) {
			case(CREATE): create(&root, 128); break;
			case(YIELD): ready(process); process = next(); break;
			case(STOP): cleanup(process); process = next(); break;
		}
		kprintf("D");
	}
}

/**
	Removes the next process from the ready queue and 
	returns an index or a pointer to its process control block
*/
struct pcb* next() {
	struct pcb* process = readyQueue;
	//kprintf("\n------ DISPATCH NEXT ------\n");
	//kprintf("\nreadyQueue1: %d; readyQueue next: %d\n", readyQueue, readyQueue->next);
	
	readyQueue = readyQueue->next;
	return process;
}

/**
	Takes an index or pointer to a process control block and 
	adds it to the ready queue
*/
void ready(struct pcb* process) {
	if (readyQueue->pid != process->pid) {
		insertNodeAtEndOfQueue(process, readyQueue);
	}
	printPcbData("ready queue", readyQueue);
}

void cleanup(struct pcb* process) {
	kfree(process->context);
	insertNodeAtEndOfQueue(process, freeQueue);
}

extern struct pcb* getFreeProcess() {
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
	struct pcb* finalQueueNode = queueBeginning->prev;
	node->next = queueBeginning;
	node->prev = finalQueueNode;
	finalQueueNode->next = node;
	queueBeginning->prev = node;
}

void printPcbData(char* info, struct pcb* node) {
	kprintf("%s pcb: %d, ->prev: %d, ->next: %d\n", info,
		node, node->prev, node->next);
}
