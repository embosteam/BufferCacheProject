 #include "linked_stack.h"
 
void* LStackTop(struct LStack* self){
        struct LStackInternalNode* current_node = self->pointer;
        if(current_node!=NULL){
            return current_node->value;
        }
        return NULL;
    }
    struct LStackInternalNode* newLStackInternalNode(void* value){
        struct LStackInternalNode* internal_node = (struct LStackInternalNode* )malloc(sizeof(struct LStackInternalNode));
        memset(internal_node,0,sizeof(struct LStackInternalNode));
        internal_node->value = value;
        return internal_node;
    }
    int LStackPush(struct LStack* self,void* value){
        if(self->count<self->fixed_capacity){
            struct LStackInternalNode* current_node = self->pointer;
            struct LStackInternalNode* next_node = newLStackInternalNode(value);
            if(current_node==NULL){
                self->pointer = next_node;
                
            }
            else{
                current_node->next = next_node;
                next_node->previous = current_node;
                self->pointer = next_node;
            }
            self->count++;
            return 1;
        }
        return 0;
    }
    int LStackPop(struct LStack* self){
        struct LStackInternalNode* current_node = self->pointer;
        if(self->count>0&&current_node!=NULL){
            struct LStackInternalNode* prev_node = current_node->previous;
            if(prev_node!=NULL){
                prev_node->next = NULL;
            }
            current_node->previous = NULL;
            free(current_node);
            self->pointer = prev_node;
            self->count--;
            return 1;
        }
        return 0;
    }
    unsigned char LStackIsEmpty(struct LStack* self){
        return self->count==0;
    }
    unsigned char LStackIsFull(struct LStack* self){
        return self->count==self->fixed_capacity;
    }
    void LStackRelease(struct LStack* self){
        while(self->isEmpty(self)){
            self->pop(self);
        }
        if(self->isLockSupported){
            sem_destroy(&self->lock);
        }
    }

    struct LStack* newLStack(unsigned int concurrent_count,unsigned int fixed_capacity){
        struct LStack* lstack = (struct LStack*)malloc(sizeof(struct LStack));
        memset(lstack,0,sizeof(struct LStack));
        lstack->isLockSupported = concurrent_count>0;
        if(lstack->isLockSupported){
            sem_init(&lstack->lock,0,concurrent_count);
        }
        lstack->top = LStackTop;
        lstack->push = LStackPush;
        lstack->pop = LStackPop;
        lstack->isEmpty = LStackIsEmpty;
        lstack->isFull = LStackIsFull;
        lstack->release = LStackRelease;
    }
    