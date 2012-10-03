/* mem.c : memory manager
 */

#include <xeroskernel.h>

long getSizeOfFirstFreeMemSlot();
long getSizeOfSecondFreeMemSlot();
long getHoleEnd();
int getAllocationSizeBytes(int);
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
const int PARAGRAPH_SIZE_BYTES = 16;

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

	hdr = createMemHeader(freemem);
	hdr->size = (unsigned long) getSizeOfFirstFreeMemSlot();
	hdr->next = (struct memHeader*) (memStart + getDataEndOfMemNode(&hdr));
	hdr->prev = 0;
	hdr->sanityCheck = &hdr->dataStart;
	
	hdr2 = createMemHeader(getHoleEnd());
	hdr2->prev = &hdr;
	hdr2->size = (unsigned long) getSizeOfSecondFreeMemSlot();
	hdr2->sanityCheck = &hdr2->dataStart;
	hdr2->next = 0;

	memSlot = &hdr;

	kprintf("\nFreemem: %d\n", freemem);
	kprintf("\nsize of memHeader: %d\n", sizeof(struct memHeader));

	kprintf("%d  dataStart %d %d %d\n", hdr, &hdr->dataStart, hdr->sanityCheck, sizeof(hdr));
  	kprintf("%d  dataStart %d %d %d\n", hdr2, &hdr2->dataStart, hdr2->sanityCheck, sizeof(struct memHeader));


	testTraverseFreeList();

	void* memPointer = kmalloc(5);
	kfree(memPointer);
	
	//testTraverseFreeList();
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
	return hdr;
}

/**
	Gets the size of the first free mem space until the beginning of the HOLE
*/
long getSizeOfFirstFreeMemSlot() {
	long HOLESTART = (640 * 1024);
	extern long freemem;

	return (long) (HOLESTART - freemem) - sizeof(struct memHeader);
}

/**
	Returns the end address of a given memory node.
*/
unsigned long getDataEndOfMemNode(struct memHeader* mem) {
	return (mem->size + sizeof(struct memHeader));
}

/**
	Calculates the size of the free memory node that comes after the memory hole
*/
long getSizeOfSecondFreeMemSlot() {
	// These variables are found in i386.c
	long HOLEEND = getHoleEnd();
	extern char	*maxaddr;	// add 1 for the number in the pdf?
	long size = (maxaddr - HOLEEND) - sizeof(struct memHeader);	
	kprintf("\nmaxAddr: %d\nHoleEnd: %d; size: %d\n", maxaddr, HOLEEND, size);
	
	return (maxaddr - HOLEEND) - sizeof(struct memHeader);
}

/**
	Finds the end address of the memory hole
*/
long getHoleEnd() {
	long HOLESIZE = 600;
	long HOLEEND = ((1024 + HOLESIZE) * 1024);
	return HOLEEND;
}


/**
	Allocates memory of a given size.
	If valid, returns pointer to the address of the allocated memory space.
	Returns -1 if the value of size is invalid
	Returns 0 if there is not enough available memory
*/
void *kmalloc(int size) {
	kprintf("\n----------KMALLOC(%d)-----------\n", size);

	//testTraverseFreeList();	

	if (size < 1) {
		// Given size is invalid
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
	unsigned char* endOfMemNode = getDataEndOfMemNode(&memAllocationSlot);
	memAllocationSlot->next = endOfMemNode;
	struct memHeader* newFreeNode = memAllocationSlot->next;

	// Set node attributes
	newFreeNode->prev = memAllocationSlot->prev;
	newFreeNode->prev->next = newFreeNode;
	memAllocationSlot->prev = 0;
	memAllocationSlot->next = 0;

	newFreeNode->next = oldNextNode;
	newFreeNode->sanityCheck = newFreeNode->dataStart;
	memAllocationSlot->sanityCheck = &memAllocationSlot->dataStart;
	updateMemListPointer(newFreeNode); // Why do I not pass through &newFreeNode??

	newFreeNode->size = (previousSize - memAllocationSlot->size) - sizeof(struct memHeader);

	kprintf("previous allocated size: %d\n", previousSize);
	kprintf("just allocated node loc: %d; size: %d\n", memAllocationSlot, memAllocationSlot->size);
	kprintf("new free node loc: %d; size: %d\n", newFreeNode, newFreeNode->size);

	// TODO: Bug?
	//	The extra nodes don't show up here, but they have still been created?
	
	return memAllocationSlot->dataStart;
}

/**
	Finds out how many paragraphs are needed to allocate for a given amount of memory,
	and then returns the result in bytes
*/
int getAllocationSizeBytes(int size) {
	int allocationSize;
	// TODO: Change to bit-masking...?
	if (size % PARAGRAPH_SIZE_BYTES == 0) {
		allocationSize = size;
	} else {
		allocationSize = (size / PARAGRAPH_SIZE_BYTES) + 1;
		allocationSize *= PARAGRAPH_SIZE_BYTES;
	}

	return allocationSize;
}


/*int getAllocationSizeBytes(int req_sz) {
	int amnt;	
	amnt = (req_sz)/16 + ((req_sz%16)?1:0);
	amnt = amnt*16 + sizeof(struct memHeader);


	return amnt;
}*/

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
		memSlot = &hdr;
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
	if ((long) closestHdr > (long) hdr) {
		// This should only happen if hdr is before the beginning of the free list
		kprintf("ClosestHdr: %d; hdr: %d\n", closestHdr, hdr);		
		hdr->next = &closestHdr;
		closestHdr->prev = &hdr;
		hdr->prev = 0;
		memSlot = &hdr;
	} else {
		//kprintf("\nIn the else\n");
		hdr->next = &closestHdr->next;
		closestHdr->next = hdr;
		//kprintf("hdr: %d; closest: %d\n", hdr, closestHdr);
		//printMemHeader(hdr);
		//printMemHeader(closestHdr);
		hdr->prev = closestHdr;
		hdr->next->prev = hdr;
	}

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

	while (((long) closestPrecedingHdr->next < hdrAddress) && currentHdr->next) {
		//kprintf("iterating: header: %d\n", currentHdr);
		closestPrecedingHdr = currentHdr;	
		currentHdr = currentHdr->next;
	}

	//kprintf("iterating: final: %d\n", currentHdr);
	//kprintf("Selected %d for the header at %d. Its size is: %d\n", 
	//	closestPrecedingHdr, hdr, closestPrecedingHdr->size);
	
	return closestPrecedingHdr;

}
