#include "linked_queue.h"
    struct LQueue* newLQueue(int concurrent_mode){
        struct LQueue* q = (struct LQueue*)malloc(sizeof(struct LQueue));
        q->size = 0;
        q->pointer = NULL;
        q->isEnableConcurrentMode = (unsigned char)(concurrent_mode>0);
        q->last_pointer = NULL;
        sem_init(&q->concurrent_lock,0,concurrent_mode);
        q->front = LQueue_front;
        q->pop = LQueue_pop;
        q->push = LQueue_push;
        q->getSize = LQueue_getSize;
        q->clear = LQueue_clear;
        q->free = LQueue_freeInternal;
        q->isEmpty = LQueue_isEmpty;
        return q;
    }
    unsigned int LQueue_getSize(struct LQueue* q){
        if(q->isEnableConcurrentMode){
            sem_wait(&q->concurrent_lock);
        }
        unsigned int size = q->size;
        if(q->isEnableConcurrentMode){
            sem_post(&q->concurrent_lock);
        }
        return size;
    }
    unsigned int LQueue_isEmpty(struct LQueue* q){
        //printf("[LQueue_isEmpty]start\n");
        if(q->isEnableConcurrentMode){
            sem_wait(&q->concurrent_lock);
        }
        unsigned int value = q->size==0;
        if(q->isEnableConcurrentMode){
            sem_post(&q->concurrent_lock);
        }
        return value;
    }
    void LQueue_clear(struct LQueue* q){
        while(!q->isEmpty(q)){
            q->pop(q);
        }
    }
    void LQueue_freeInternal(struct LQueue* q){
        q->clear(q);
        sem_destroy(&q->concurrent_lock);

    }
    void* LQueue_front(struct LQueue* q)
    {
        if(q->isEnableConcurrentMode){
            sem_wait(&q->concurrent_lock);
        }
        struct LQueueInternalNode* ptr = q->pointer;
        if(q->isEnableConcurrentMode){
            sem_post(&q->concurrent_lock);
        }
        if(ptr!=NULL){
            return ptr->value;
        }
        else{
            return NULL;
        }
    }
    void LQueue_pop(struct LQueue* q){
        if(q->isEnableConcurrentMode){
            sem_wait(&q->concurrent_lock);
        }
        struct LQueueInternalNode* ptr = q->pointer;
        unsigned int size = q->size;
        if(ptr!=NULL){
            if(size==1){
                q->pointer = NULL;
                q->last_pointer = NULL;
            }
            else{
               q->pointer = ptr->next;
            }
            free(ptr);
            q->size--;
        }
        else{
            //free(ptr);
        }
        if(q->isEnableConcurrentMode){
            sem_post(&q->concurrent_lock);
        }
    }
    struct LQueueInternalNode* LQueue_newEmptyInternalNode(){
        struct LQueueInternalNode* node = (struct LQueueInternalNode*)malloc(sizeof(struct LQueueInternalNode));
        node->next = NULL;
        node->value = NULL;
        return node;
    }
    void LQueue_push(struct LQueue* q,void* value){
         if(q->isEnableConcurrentMode){
            sem_wait(&q->concurrent_lock);
        }
         struct LQueueInternalNode* last_ptr = q->last_pointer;
        //struct LQueueInternalNode* ptr = q->pointer;
        struct LQueueInternalNode* node = LQueue_newEmptyInternalNode();
         node->value = value;
        if(last_ptr==NULL){
            
            q->last_pointer = node;
            q->pointer = node;
        }
        else{
            q->last_pointer->next =  node;
            q->last_pointer = node;
        }
        q->size++;
        if(q->isEnableConcurrentMode){
            sem_post(&q->concurrent_lock);
        }
    }