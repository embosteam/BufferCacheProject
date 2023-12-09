//#pragma once
#ifndef __HEADER_LINKED_QUEUE__
    #define __HEADER_LINKED_QUEUE__
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <semaphore.h>
    #include <memory.h>
    struct LQueueInternalNode{
        struct LQueueInternalNode* next;
        void* value;
    };
    struct LQueue{
        unsigned char isEnableConcurrentMode:1;
        unsigned int size;

        sem_t concurrent_lock;
        struct LQueueInternalNode* pointer;
        struct LQueueInternalNode* last_pointer;
        void* (*front)(struct LQueue*);
        void (*pop)(struct LQueue*);
        unsigned int (*getSize)(struct LQueue*);
        unsigned int (*isEmpty)(struct LQueue*);
        void (*push)(struct LQueue*,void*);
        void (*clear)(struct LQueue*);
        void (*free)(struct LQueue*);
        
    };
    struct LQueue* newLQueue( int concurrent_mode);
    unsigned int LQueue_getSize(struct LQueue* q);
    unsigned int LQueue_isEmpty(struct LQueue* q);
    void LQueue_clear(struct LQueue* q);
    void LQueue_freeInternal(struct LQueue* q);
    void* LQueue_front(struct LQueue* q);
    void LQueue_pop(struct LQueue* q);
    //struct LQueueInternalNode* LQueue_newEmptyInternalNode();
    void LQueue_push(struct LQueue* q,void* value);
    
#endif