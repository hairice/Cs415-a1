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



extern int send(unsigned int dest_pid, void *buffer, int buffer_len, pcb* sndProc) {
    kprintf("receiver pid: %d sender pid: %d\n", dest_pid, getCurrentPid());
    
    placeBufferDataOnStack(sndProc, buffer, buffer_len);
    
    if (isReceiverWaiting(dest_pid)) {
        kprintf("Receiver is waiting\n");
        putSendDataOnRecvStack(dest_pid, buffer, buffer_len);
        ready(getProcessByPid(dest_pid));
        ready(sndProc);
        return buffer_len;
    } else {
        // for some reason if I remove the line below everything stops working...
        kprintf("not in the else block\n");
        pcb* receiver = getProcessByPid(dest_pid);
        addProcessToSenderQueue(sndProc, receiver);
        kprintf("Out of func. rece->sender: %d, pid: %d\n", 
                receiver->senderQueue, receiver->pid);
        block(sndProc);
        return buffer_len;
    }
}

extern int recv(pcb* receiver, unsigned int* from_pid, void* buffer, int buffer_len) {
    kprintf("in recv\n");
    kprintf("sender pid: %d, sender queue: %d, receiver: %d, int(pid): %d\n", 
        from_pid, receiver->senderQueue, receiver->pid, (int) from_pid);
    
    if (!receiver->senderQueue) {
        kprintf("No sender queue\n");
        placeBufferDataOnStack(receiver, buffer, buffer_len);
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

void putSendDataOnRecvStack(unsigned int dest_pid, void* buffer, int buffer_len) {
    pcb* recvProc = getProcessByPid(dest_pid);
    unsigned int* recvStackLoc = (unsigned int*) recvProc->esp - 1;
    int recvBufferLen = *recvStackLoc;
    kprintf("senderStackLoc: %d, *: %d\n", recvStackLoc, *recvStackLoc);

    recvStackLoc++;
    kprintf("recvBufferLen: %d, msgLoc: %d\n", recvBufferLen, recvStackLoc);

    // Put data in the receiver's buffer
    unsigned int* recvBuffer = (unsigned int*) recvStackLoc;
    char* recvBufferData = (char*) *recvBuffer;
    *recvBufferData = (char*) buffer;
    
    kprintf("recvBuffLen: %d, recvBufferData: %s\n", recvBufferLen, (char*) buffer);
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
