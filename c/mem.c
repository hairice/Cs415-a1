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

extern void testKmalloc();
extern void testMemInit();


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

    //printMemLocInfo();

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
    extern long freemem;
    long size = HOLESTART - freemem - sizeof (struct memHeader);
    return size;
}

/**
        Returns the end address of a given memory node.
 */
unsigned long getDataEndOfMemNode(struct memHeader* mem) {
    unsigned long endAddr = ((long) &mem->dataStart) + mem->size;
    return endAddr;
}

/**
        Calculates the size of the free memory node that comes after the memory hole
 */
long getSizeOfSecondFreeMemSlot() {
    extern char *maxaddr;
    long size = (maxaddr - HOLEEND) - sizeof (struct memHeader);
    return (maxaddr - HOLEEND) - sizeof (struct memHeader);
}

/**
        Allocates memory of a given size.
        If valid, returns pointer to the address of the allocated memory space.
        Returns -1 if the value of size is invalid
        Returns 0 if there is not enough available memory
 */
void *kmalloc(int size) {
    //kprintf("\n----------KMALLOC(%d)-----------\n", size);
    //testTraverseFreeList();

    if (!isAllocSizeValid(size)) {
        return -1;
    }

    int allocationSize = getAllocationSizeBytes(size);

    //kprintf("Size requested: %d; need to allocate: %d\n", size, allocationSize);

    struct memHeader* memAllocationSlot = traverseFreeList(allocationSize);

    //kprintf("First hdr with enough space: %d\n", memAllocationSlot);

    //sleep();

    if (!memAllocationSlot) {
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
    newFreeNode->next->prev = newFreeNode;
    newFreeNode->sanityCheck = newFreeNode->dataStart;
    memAllocationSlot->sanityCheck = memAllocationSlot->dataStart;
    updateMemListPointer(newFreeNode);

    newFreeNode->size = (previousSize - memAllocationSlot->size) - sizeof (struct memHeader);

    // Point start of free memory list to the newly free node
    memSlot = newFreeNode;

    //kprintf("just allocated node loc: %d\n", memAllocationSlot);
    //printMemHeader(memAllocationSlot);
    //kprintf("New free node: ");
    //printMemHeader(newFreeNode);
    //kprintf("Post-allocation list traversal:\n");
    //testTraverseFreeList();

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
    amnt = (req_sz) / 16 + ((req_sz % 16) ? 1 : 0);
    amnt = amnt * 16 + sizeof (struct memHeader);

    return amnt;
}

/**
        Traverses the free memroy list for a free slot that can accomodate the given amount
        of bytes. Returns the corresponding node if one exists, else 0.
 */
struct memHeader* traverseFreeList(int size) {
    struct memHeader* currentMemSlot = memSlot;

    while (currentMemSlot->next) {
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
    //testTraverseFreeList();
    struct memHeader* hdr = createMemHeader(ptr - sizeof (struct memHeader));
    long sanityCheck1 = ptr;
    long sanityCheck2 = hdr->sanityCheck;

    if (sanityCheck1 != sanityCheck2) {
        //kprintf("Sanity check doesn't match - %d vs. %d\n", sanityCheck1, sanityCheck2);
        return -1;
    }

    //kprintf("The header we want to free: %d\n", hdr);
    //kprintf("Freeing %d bytes\n", hdr->size);

    placeMemInFreeList(hdr);

    //testTraverseFreeList();
}

/**
        Places the node in order in the free list
 */
void placeMemInFreeList(struct memHeader *hdr) {
    struct memHeader* closestHdr = getClosestHdrFromFreeList(hdr);
    if ((long) closestHdr > (long) hdr) {
        // This should only happen if hdr is before the beginning of the free list
        //kprintf("we are freeing the lowest header: %d\n", hdr);		
        hdr->next = closestHdr;
        closestHdr->prev = hdr;
        hdr->prev = 0;
        memSlot = hdr;
    } else {
        //kprintf("closest previous header: %d\n", closestHdr);
        hdr->next = closestHdr->next;
        closestHdr->next = hdr;
        hdr->prev = closestHdr;
        hdr->next->prev = hdr;
    }

    hdr->sanityCheck = hdr->dataStart;
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
        closestPrecedingHdr = currentHdr;
        currentHdr = currentHdr->next;
    }

    return closestPrecedingHdr;

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

extern void testMemInit() {
    extern long freemem;
    extern char *maxaddr;
    kprintf("\nmemSlot: %d\n", memSlot);
    kprintf("\nFreemem: %d Holestart: %d HoleEnd: %d maxaddr: %d\n", freemem, HOLESTART, HOLEEND, maxaddr);
    kprintf("\nsize of memHeader: %d; holesize: %d\n", sizeof (struct memHeader), HOLEEND - HOLESTART);

    testTraverseFreeList();
}

extern void testKmalloc() {
    struct memHeader* h1 = kmalloc(5);
    kfree(h1);
}

void printMemHeader(struct memHeader* hdr) {
    kprintf("\nhdr %d:\n\thdr->size: %d; hdr->prev: %d; hdr->next:%d;\nhdr->dataStart: %d; hdr->sanity: %d\n",
            hdr, hdr->size, hdr->prev, hdr->next, &hdr->dataStart, hdr->sanityCheck);
}

void sleep() {
    int i;
    for (i = 0; i < 10000000; i++);
}

void printMemLocInfo() {
    kprintf("Mem loc services: %d - %d\n", &kmeminit, &printMemHeader);
}
