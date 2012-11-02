/* msg.c : messaging system (assignment 2)
 */

#include <xeroskernel.h>

/* Your code goes here */

short isReceiverWaiting(unsigned int receiverPid);
void addProcessToSenderQueue(pcb* sender, pcb* receiver);



extern int send(unsigned int dest_pid, void *buffer, int buffer_len, pcb* sndProc) {
    kprintf("receiver pid: %d sender pid: %d\n", dest_pid, getCurrentPid());

    unsigned int* stackLoc = (unsigned int*) sndProc->esp;
    *stackLoc = buffer;
    //stackLoc += buffer_len;
    stackLoc--;
    *stackLoc = buffer_len;
    stackLoc--;
    
    if (isReceiverWaiting(dest_pid)) {
        kprintf("Receiver is waiting\n");
        doSend();
    } else {
        pcb* receiver = getProcessByPid(dest_pid);
        addProcessToSenderQueue(sndProc, receiver);
        kprintf("Out of func. rece->sender: %d, pid: %d\n", 
                receiver->senderQueue, receiver->pid);
        block(sndProc);
    }
    
    return buffer_len;
}

extern int recv(pcb* receiver, unsigned int* from_pid, void* buffer, int buffer_len) {
    kprintf("in recv\n");
    kprintf("sender pid: %d, sender queue: %d, receiver: %d, int(pid): %d\n", 
        from_pid, receiver->senderQueue, receiver->pid, (int) from_pid);
    
    if (!receiver->senderQueue) {
        kprintf("No sender queue\n");
        block(receiver);
        return 0;
    }
    pcb* sender = getProcessByPid(from_pid);
    kprintf("sender pid check: %d\n", sender->pid);
    unsigned int* senderStackLoc = (unsigned int*) sender->esp - 1;

    kprintf("senderStackLoc: %d, *: %d\n", senderStackLoc, *senderStackLoc);

    int sendBufferLen = *senderStackLoc;

    senderStackLoc++;
    kprintf("sendBufferLen: %d, msgLoc: %d\n", sendBufferLen, senderStackLoc);

    // Put data in the receiver's buffer
    char* bufferData = (char*) buffer;
    *bufferData = (char*) *senderStackLoc;

    kprintf("buffer length: %d, received message: %s\n", sendBufferLen, (char*) *senderStackLoc);

    // adjust receiver's sender queue
    pcb* temp = &receiver->senderQueue->next;
    ready(receiver->senderQueue);
    receiver->senderQueue = temp;
    
    return buffer_len;
}





void doSend() {
    
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
    if (receiver->senderQueue == NULL) {
        receiver->senderQueue = sender;
    } else {
        pcb* currPcb = receiver->senderQueue;
        
        while(currPcb->next != NULL) {
            currPcb = currPcb->next;
        }
        
        sender->prev = currPcb;
        currPcb->next = sender;
    }
    kprintf("sender: %d, receiver: %d, rec->senders: %d\n", 
        sender, receiver, receiver->senderQueue);
    testTraverseQueue(receiver);
}