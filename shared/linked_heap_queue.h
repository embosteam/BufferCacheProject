#ifndef __HEADER_LINKED_HEAP_QUEUE__
    #define __HEADER_LINKED_HEAP_QUEUE__
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <semaphore.h>
    #include <memory.h>
    #include "linked_heap.h"
    
    struct LHQueue{
        unsigned char isEnableConcurrentMode:1;
        unsigned int size;

        struct LHeap* heap;
        
        void* (*front)(struct LHQueue*);
        void (*pop)(struct LHQueue*);
        unsigned int (*getSize)(struct LHQueue*);
        unsigned int (*isEmpty)(struct LHQueue*);
        void (*push)(struct LHQueue*,void*);
        void (*clear)(struct LHQueue*);
        void (*free)(struct LHQueue*);
        
    };
    struct LHQueue* newLHQueue( int concurrent_mode,int (*fetchKeyFromValue)(struct LHeap*,void*),int (*comparator)(struct LHeap*,void*,void*));
    struct LHQueue* newLHQueue2( int concurrent_mode,int (*fetchKeyFromValue)(struct LHeap*,void*));

    unsigned int LHQueue_getSize(struct LHQueue* q);
    unsigned int LHQueue_isEmpty(struct LHQueue* q);
    void LHQueue_clear(struct LHQueue* q);
    void LHQueue_freeInternal(struct LHQueue* q);
    void* LHQueue_front(struct LHQueue* q);
    void LHQueue_pop(struct LHQueue* q);
    //struct LHQueueInternalNode* LQueue_newEmptyInternalNode();
    void LHQueue_push(struct LHQueue* q,void* value);
    
#endif