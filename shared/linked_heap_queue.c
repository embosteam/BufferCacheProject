#include "linked_heap_queue.h"
unsigned int LHQueue_getSize(struct LHQueue* q){
    return q->heap->getCount(q->heap);
}

    unsigned int LHQueue_isEmpty(struct LHQueue* q){
        return q->heap->isEmpty(q->heap);
    }
    void LHQueue_clear(struct LHQueue* q){
        q->heap->clear(q->heap);
    }
    void LHQueue_freeInternal(struct LHQueue* q){
       // q->heap->release(q->heap);
    }
    void* LHQueue_front(struct LHQueue* q){
        return q->heap->get(q->heap);
    }
    void LHQueue_pop(struct LHQueue* q){
        q->heap->pop(q->heap);
    }
    //struct LHQueueInternalNode* LQueue_newEmptyInternalNode();
    void LHQueue_push(struct LHQueue* q,void* value){
        q->heap->push(q->heap,value);
    }
struct LHQueue* newLHQueue( int concurrent_mode,int (*fetchKeyFromValue)(struct LHeap*,void*),int (*comparator)(struct LHeap*,void*,void*)){
    struct LHQueue* q = (struct LHQueue*)malloc(sizeof(struct LHQueue));
    memset(q,0,sizeof(struct LHQueue));
    q->front = LHQueue_front;
        q->pop = LHQueue_pop;
        q->push = LHQueue_push;
        q->getSize = LHQueue_getSize;
        q->clear = LHQueue_clear;
        q->free = LHQueue_freeInternal;
        q->isEmpty = LHQueue_isEmpty;
        q->heap = newLHeap(concurrent_mode,fetchKeyFromValue,comparator);
        return q;
}
    struct LHQueue* newLHQueue2( int concurrent_mode,int (*fetchKeyFromValue)(struct LHeap*,void*)){
        struct LHQueue* q = (struct LHQueue*)malloc(sizeof(struct LHQueue));
    memset(q,0,sizeof(struct LHQueue));
    q->front = LHQueue_front;
        q->pop = LHQueue_pop;
        q->push = LHQueue_push;
        q->getSize = LHQueue_getSize;
        q->clear = LHQueue_clear;
        q->free = LHQueue_freeInternal;
        q->isEmpty = LHQueue_isEmpty;
        q->heap = newLHeap2(concurrent_mode,fetchKeyFromValue);
        return q;
    }

    