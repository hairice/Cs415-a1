/* mem.c : memory manager
 */

#include <xeroskernel.h>
#include <i386.h>

long getSizeOfFirstFreeMemSlot();
long getSizeOfSecondFreeMemSlot();
int getAllocationSizeBytes(int);
short isAllocSizeValid(int);
struct memHeader* traverseFreeList(int);
struct memHeader* createMemHeader(long);
void* kmalloc(int);
unsigned long getDataEndOfMemNode(struct memHeader*);
void updateMemListPointer(struct memHeader* hdr);
void kfree(void *ptr);
void placeMemInFreeList(struct memHeader* hdr);
struct memHeader* getClosestHdrFromFreeList(struct memHeader* hdr);
void testTraverseFreeList();


static struct memHeader *memSlot;

struct memHeader {
  unsigned long size;
  struct memHeader *prev;
  struct memHeader *next;
  char* sanityCheck;
  unsigned char dataStart[0];
};

/* Your code goes here */

/**
	Performs any initialization that the memory manager requires
*/
void kmeminit(void) {
	extern long freemem;
	unsigned char* memStart = (unsigned char*) freemem;
	struct memHeader *hdr, *hdr2;


	// You are having address issues.
	hdr = createMemHeader(freemem);
	hdr->size = (unsigned long) getSizeOfFirstFreeMemSlot();
	hdr->prev = 0;
	hdr->sanityCheck = hdr->dataStart;
	
	hdr2 = createMemHeader(HOLEEND);
	hdr2->prev = hdr;
	hdr2->size = (unsigned long) getSizeOfSecondFreeMemSlot();
	hdr2->sanityCheck = hdr2->dataStart;
	hdr2->next = 0;

	hdr->next = hdr2;
	memSlot = hdr;
	kprintf("\nmemSlot: %d\n", memSlot);
	kprintf("\nFreemem: %d\n", freemem);
	kprintf("\nsize of memHeader: %d; holesize: %d\n", sizeof(struct memHeader), HOLEEND - HOLESTART);

	kprintf("%d  dataStart %d %d %d %d\n", hdr, &hdr->dataStart, hdr->sanityCheck, sizeof(hdr), hdr->next);
  	kprintf("%d  dataStart %d %d %d\n", hdr2, &hdr2->dataStart, hdr2->sanityCheck, sizeof(struct memHeader));
	
	testTraverseFreeList();
}

void testTraverseFreeList() {
	struct memHeader* currentHeader;
	currentHeader = memSlot;
	while (currentHeader->next) {
		printMemHeader(currentHeader);
		currentHeader = currentHeader->next;
	}	
		kprintf("final: ");
		printMemHeader(currentHeader);
		kprintf("\nmemslot: %d\n", memSlot);
}

void printMemHeader(struct memHeader* hdr) {
	kprintf("\nhdr %d:\n\thdr->size: %d; hdr->prev: %d; hdr->next:%d;\nhdr->dataStart: %d; hdr->sanity: %d\n",
		hdr, hdr->size, hdr->prev, hdr->next, &hdr->dataStart, hdr->sanityCheck);
}

void sleep() {
	int i;
	for (i = 0; i < 10000000; i++);
}

/**
	Creates a memory header at a given memory location and returns it.
*/
struct memHeader* createMemHeader(long memLoc) {
	struct memHeader* hdr;	
	hdr = (struct memHeader*) (unsigned char*) memLoc;
	kprintf("createMemHeader: %d\n", hdr);
	return hdr;
}

/**
	Gets the size of the first free mem space until the beginning of the HOLE
*/
long getSizeOfFirstFreeMemSlot() {
	extern long freemem;
	long size = HOLESTART - freemem - sizeof(struct memHeader);
	kprintf("Holestart: %d; Size of first free mem slot: %d\n", HOLESTART, size);
	return size;
}

/**
	Returns the end address of a given memory node.
*/
unsigned long getDataEndOfMemNode(struct memHeader* mem) {
	unsigned long endAddr = ((long) &mem->dataStart) + mem->size;
	kprintf("mem size: %d; mem: %d; Mem node end addr: %d\n", mem->size, mem, endAddr);	
	return endAddr;
}

/**
	Calculates the size of the free memory node that comes after the memory hole
*/
long getSizeOfSecondFreeMemSlot() {
	extern char	*maxaddr;	// add 1 for the number in the pdf?
	long size = (maxaddr - HOLEEND) - sizeof(struct memHeader);	
	kprintf("\nmaxAddr: %d\nHoleEnd: %d; size: %d\n", maxaddr, HOLEEND, size);
	
	return (maxaddr - HOLEEND) - sizeof(struct memHeader);
}

/**
	Allocates memory of a given size.
	If valid, returns pointer to the address of the allocated memory space.
	Returns -1 if the value of size is invalid
	Returns 0 if there is not enough available memory
*/
void *kmalloc(int size) {
	kprintf("\n----------KMALLOC(%d)-----------\n", size);

	if (!isAllocSizeValid(size)) {
		return -1;
	}
	
	int allocationSize = getAllocationSizeBytes(size);
	struct memHeader* memAllocationSlot = traverseFreeList(allocationSize);
	
	if (!memAllocationSlot) {
		// There wasn't a free slot with enough space
		kprintf("Not enough space to kmalloc()!");
		return 0;
	}

	// Store the free node's old attributes and set the new ones
	unsigned long previousSize = memAllocationSlot->size;
	memAllocationSlot->size = allocationSize;
	struct memHeader* oldNextNode = memAllocationSlot->next;

	// Create new free node where newly filled node ends.
	unsigned char* endOfMemNode = (unsigned char*) getDataEndOfMemNode(memAllocationSlot);
	memAllocationSlot->next = endOfMemNode;
	struct memHeader* newFreeNode = memAllocationSlot->next;

	// Set node attributes
	newFreeNode->prev = memAllocationSlot->prev;
	newFreeNode->prev->next = newFreeNode;
	memAllocationSlot->prev = 0;
	memAllocationSlot->next = 0;

	newFreeNode->next = oldNextNode;
	newFreeNode->sanityCheck = newFreeNode->dataStart;
	memAllocationSlot->sanityCheck = memAllocationSlot->dataStart;
	updateMemListPointer(newFreeNode); // Why do I not pass through &newFreeNode??

	newFreeNode->size = (previousSize - memAllocationSlot->size) - sizeof(struct memHeader);

	// Point start of free memory list to the newly free node
	memSlot = newFreeNode;

	kprintf("previous allocated size: %d\n", previousSize);
	kprintf("just allocated node loc: %d; size: %d\n", memAllocationSlot, memAllocationSlot->size);
	kprintf("new free node loc: %d; size: %d\n", newFreeNode, newFreeNode->size);
	
	return memAllocationSlot->dataStart;
}

short isAllocSizeValid(int size) {
	return size >= 1;
}

/**
	Finds out how many paragraphs are needed to allocate for a given amount of memory,
	and then returns the result in bytes
*/
int getAllocationSizeBytes(int req_sz) {
	int amnt;
	amnt = (req_sz)/16 + ((req_sz%16)?1:0);
	amnt = amnt*16 + sizeof(struct memHeader);

	return amnt;
}

/**
	Traverses the free memroy list for a free slot that can accomodate the given amount
	of bytes. Returns the corresponding node if one exists, else 0.
*/
struct memHeader* traverseFreeList(int size) {
	struct memHeader* currentMemSlot = memSlot;
	
	while(currentMemSlot->next) {
		if (currentMemSlot->size >= size) {
			return currentMemSlot;
		} else {
			currentMemSlot = currentMemSlot->next;
		}
	}

	return 0;
}

/**
	Updates the free memory list pointer if necessary
*/
void updateMemListPointer(struct memHeader* hdr) {
	if (!hdr->prev) {
		memSlot = hdr;
	}
}

void kfree(void *ptr) {
	struct memHeader* hdr = createMemHeader(ptr - sizeof(struct memHeader));
	long sanityCheck1 = ptr;
	long sanityCheck2 = hdr->sanityCheck;
	
	if (sanityCheck1 != sanityCheck2) {
		kprintf("\n---------SANITY CHECK DOES NOT MATCH----------");
		kprintf("sanityCheck1: %d; sanityCheck2: %d\n", sanityCheck1, sanityCheck2);
	}

	kprintf("Freeing %d bytes\n", hdr->size);

	placeMemInFreeList(ptr);
}

/**
	Places the node in order in the free list
*/
void placeMemInFreeList(struct memHeader *hdr) {
	struct memHeader* closestHdr = getClosestHdrFromFreeList(hdr);
	kprintf("Free mem list: hdr: %d; closest: %d\n", hdr, closestHdr);
	if ((long) closestHdr > (long) hdr) {
		// This should only happen if hdr is before the beginning of the free list
		hdr->next = closestHdr;
		closestHdr->prev = hdr;
		hdr->prev = 0;
		memSlot = hdr;
	} else {
		hdr->next = &closestHdr->next;
		closestHdr->next = hdr;
		hdr->prev = closestHdr;
		hdr->next->prev = hdr;
	}

	hdr->sanityCheck = hdr->dataStart;

	//testTraverseFreeList();
}

/**
	Traverses the free memory list looking for the header that most closely precedes
	the given header. If there is nothing preceding, a pointer to the beginning of
	the free list is returned. 
*/
struct memHeader* getClosestHdrFromFreeList(struct memHeader* hdr) {
	struct memHeader* currentHdr = memSlot;
	struct memHeader* closestPrecedingHdr = currentHdr;
	long hdrAddress = (long) hdr;
	kprintf("getting closest: hdr: %d; memslot: %d; current: %d; closest: %d\n", hdr, memSlot, currentHdr, closestPrecedingHdr);

	while (((long) closestPrecedingHdr->next < hdrAddress) && currentHdr->next) {
		//kprintf("iterating: header: %d\n", currentHdr);
		closestPrecedingHdr = currentHdr;	
		currentHdr = currentHdr->next;
	}

	//kprintf("iterating: final: %d\n", currentHdr);
	kprintf("Selected %d for the header at %d. Its size is: %d\n", 
		closestPrecedingHdr, hdr, closestPrecedingHdr->size);
	
	return closestPrecedingHdr;

}
