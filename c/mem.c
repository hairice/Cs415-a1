/* mem.c : memory manager
 */

#include <xeroskernel.h>
#include <i386.h>

/* Your code goes here */

/*Why do you need a paragraph mask?*/
#define PARAGRAPH_MASK  (~(0xf))

extern long freemem;

typedef struct struct_mem mem;

struct struct_mem {
    mem *next;
    mem *prev;
    unsigned int size;
    unsigned char dataStart[0];
};

void placeMemInFreeList(mem*);
mem* getClosestHdrFromFreeList(mem*);
mem* createMemHeader(long);

static mem *head;

void kmeminit(void) {
    /****************************/

    long s;

    s = (freemem + 0x10) & PARAGRAPH_MASK;

    head = (mem *) s;
    head->size = HOLESTART - s;
    head->prev = NULL;

    s = HOLEEND;

    head->next = (mem *) s;
    head->next->next = NULL;
    head->next->prev = head;
    head->next->size = (1024 * 1024 * 4) - HOLEEND;
}

void kfree(void * memPointer) {
    
    mem* hdr = createMemHeader(memPointer - sizeof (mem));
    //kprintf("Freeing mem of size %d\n", hdr->size);
    placeMemInFreeList(hdr);
    
    kprintf("size: %d, next hdr: %d, prev hdr: %d, next hdr size: %d, prev size: %d\n", 
        hdr->size, hdr->next, hdr->prev, hdr->next->size, hdr->prev->size);

}

mem* createMemHeader(long memLoc) {
    mem* hdr;
    hdr = (mem*) (unsigned char*) memLoc;
    return hdr;
}

/**
        Places the node in order in the free list
 */
void placeMemInFreeList(mem *hdr) {
    mem* closestHdr = getClosestHdrFromFreeList(hdr);
    if ((long) closestHdr > (long) hdr) {
        // This should only happen if hdr is before the beginning of the free list
        //kprintf("we are freeing the lowest header: %d\n", hdr);		
        hdr->next = closestHdr;
        closestHdr->prev = hdr;
        hdr->prev = 0;
        head = hdr;
    } else {
        //kprintf("closest previous header: %d\n", closestHdr);
        hdr->next = closestHdr->next;
        closestHdr->next = hdr;
        hdr->prev = closestHdr;
        hdr->next->prev = hdr;
    }
}

/**
        Traverses the free memory list looking for the header that most closely precedes
        the given header. If there is nothing preceding, a pointer to the beginning of
        the free list is returned. 
 */
mem* getClosestHdrFromFreeList(mem* hdr) {
    mem* currentHdr = head;
    mem* closestPrecedingHdr = currentHdr;
    long hdrAddress = (long) hdr;
    while (((long) closestPrecedingHdr->next < hdrAddress) && currentHdr->next) {
        closestPrecedingHdr = currentHdr;
        currentHdr = currentHdr->next;
    }

    return closestPrecedingHdr;
}

void *kmalloc(int size) {
    /********************************/

    mem *p;
    mem *r;

    if (size & 0xf) {
        size = (size + 0x10) & PARAGRAPH_MASK;
    }

    for (p = head; p && (p->size < size); p = p->next);

    if (!p) {
        return ( 0);
    }
    
    p->size = p->size - size;
    kprintf("old node's psize: %d, size: %d\n", p->size, size);

    if (p->size < sizeof (mem)) {
        if (p->next) {
            p->next->prev = p->prev;
        }

        if (p->prev) {
            p->prev->next = p->next;
        } else {
            head = p->next;
        }
    } else {
        r = (mem *) ((int) p + size);
        *r = *p;
        
        if (p->next) {
            p->next->prev = r;
        }

        if (p->prev) {
            p->prev->next = r;
        } else {
            head = r;
        }
    }

    p->size = (unsigned int) size;
    kprintf("new node's psize: %d, size: %d\n", p->size, size);

    
    return ( p);
}
