/* msg.c : messaging system (assignment 2)
 */

#include <xeroskernel.h>

/* Your code goes here */

/*
 * TODO: make those checks
 * TODO: implement a dest/snd pid of 0
 */


short isReceiverWaiting(unsigned int receiverPid);
void addProcessToSenderQueue(pcb* sender, pcb* receiver);


/**
 * Implements the kernel side of syssend. Blocks the sending process until the
 * operation is complete.
 * @param dest_pid  the pid of the receiving process
 * @param buffer  the data to be sent
 * @param buffer_len  the length of the data to be sent
 * @param sndProc  the sending process
 * @return  the number of bytes accepted by the receiving process or a negative
 *  value if the operation failed.
 * 
 * @see syssend(int, void*, int)
 */
extern int send(unsigned int dest_pid, void *buffer, int buffer_len, pcb* sndProc) {
    //kprintf("receiver pid: %d sender pid: %d\n", dest_pid, getCurrentPid());
    
    placeBufferDataOnStack(sndProc, buffer, buffer_len);
    
    if (isReceiverWaiting(dest_pid)) {
      //  kprintf("Receiver is waiting\n");
        putDataOnRecvStack(dest_pid, buffer, buffer_len);
        ready(getProcessByPid(dest_pid));
        ready(sndProc);
        return buffer_len;
    } else {
        //kprintf("No waiting receiver :(\n");
        pcb* receiver = getProcessByPid(dest_pid);
        addProcessToSenderQueue(sndProc, receiver);
        //kprintf("Added process to senderQueue. recv->senderQueue: %d, pid: %d\n", 
        //        receiver->senderQueue, receiver->pid);
        block(sndProc);
        return buffer_len;
    }
}

int getReceiveBufferLength(unsigned int dest_pid) {
    pcb* receiver = getProcessByPid(dest_pid);
    unsigned int* recvBufferLoc = (unsigned int*) receiver->esp - 1;
    int recvBufferLen = *recvBufferLoc;
    //kprintf("recvBufferLen = %d\n", recvBufferLen);
    return recvBufferLen;
}


/**
 * Implements the kernel side of sysrecv. 
 * 
 * Sets the process up to receive a message. If no messages are available, the
 * process is blocked until one is sent to it.
 * 
 * Returns the number of bytes that were received OR
 *      -1 if the PID to receive from is invalid
 *      -2 if the process tries to receive from itself
 *      -3 for any other problem (eg. size is negative)
 * @param from_pid - contains the address of the pid the process is receiving from
 * @param buffer - address of the destination buffer
 * @param buffer_len - length of the buffer
 * @return number of bytes successfully received
 **/ 
extern int recv(pcb* receiver, unsigned int* from_pid, void* buffer, int buffer_len) {
    //kprintf("in recv\n");
    
    if (!receiver->senderQueue) {
        //kprintf("No sender queue - putting data on stack and blocking\n");
        placeBufferDataOnStack(receiver, buffer, buffer_len);
        block(receiver);
        return 0;
    } else {
        //kprintf("sender queue waiting - traversing\n");
        //testTraverseQueue(receiver->senderQueue);
    }
    
    pcb* sender;
    if ((int) from_pid == 0) {
      //  kprintf("receiving message from any process!\n");
        sender = receiver->senderQueue;
    }
    else {
        sender = getProcessByPid(from_pid);
    }
    
    unsigned int* senderStackLoc = (unsigned int*) sender->esp - 1;
    int sendBufferLen = *senderStackLoc;

    senderStackLoc++;
    //kprintf("sendBufferLen: %d, msgLoc: %d, msg: %d\n", sendBufferLen, senderStackLoc, *senderStackLoc);

    // Put data in the receiver's buffer
    int* bufferData = (int*) buffer;
    *bufferData = (int*) *senderStackLoc;
    
    // adjust receiver's sender queue
    pcb* temp = receiver->senderQueue->next;
    temp->prev = NULL;
    ready(receiver->senderQueue);
    receiver->senderQueue = &temp;
   
    return buffer_len;
}



void placeBufferDataOnStack(pcb* process, void* buffer, int buffer_len) {
    unsigned int* stackLoc = (unsigned int*) process->esp;
    *stackLoc = buffer;
    stackLoc--;
    *stackLoc = buffer_len;
    stackLoc--;
}

/**
 * Puts the given data on to the stack of the process indicated by a given pid
 * @param dest_pid
 * @param buffer  the data to copy
 * @param buffer_len  the amount of data to copy
 */
void putDataOnRecvStack(unsigned int dest_pid, void* buffer, int buffer_len) {
    pcb* recvProc = getProcessByPid(dest_pid);
    unsigned int* recvStackLoc = (unsigned int*) recvProc->esp - 1;
    int recvBufferLen = *recvStackLoc;
    //kprintf("senderStackLoc: %d, *: %d\n", recvStackLoc, *recvStackLoc);

    recvStackLoc++;

    // Put data in the receiver's buffer
    int* recvBuffer = (int*) *recvStackLoc;
    *recvBuffer = (int*) buffer;
    
    if (recvBufferLen < buffer_len) recvBufferLen = buffer_len;
    
/*
    kprintf("stackLoc %d, *stackLoc %d, buffer contents %d\n", recvStackLoc, *recvStackLoc, buffer);
    kprintf("recvBuffLen: %d, recvBufferContents: %d\n", recvBufferLen, buffer);
*/
}

extern context_frame* getProcessContext(pcb* proc) {
    context_frame* context = proc->esp;
    return context;
}

short isReceiverWaiting(unsigned int receiverPid) {
    pcb* p = getProcessByPid(receiverPid);
    while (p != NULL && p->pid != receiverPid) {
        p = p->next;
    }
    
    if (p->state == STATE_BLOCKED) return 1;
    else return 0;
}

void addProcessToSenderQueue(pcb* sender, pcb* receiver) {
    //kprintf("adding process %d to the senderQueue of receiver %d\n", sender->pid, receiver->pid);
    if (receiver->senderQueue == NULL) {
        receiver->senderQueue = sender;
        receiver->senderQueue->next = NULL;
        receiver->senderQueue->prev = NULL;
    } else {
        pcb* currPcb = receiver->senderQueue;
        
        while(currPcb->next != NULL) {
            currPcb = currPcb->next;
        }
        
        sender->prev = currPcb;
        currPcb->next = sender;
    }
    //kprintf("traversing receiver send queue - sender %d, receiver %d\n", sender, receiver);
    //testTraverseQueue(receiver->senderQueue);
}
