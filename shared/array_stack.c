 #include "array_stack.h"
 
    void* AStackTop(struct AStack* self){
        if(self->isLockSupported){
            sem_wait(&self->lock);
        }
        void* current_node = self->pointer[self->count];
        if(self->isLockSupported){
            sem_post(&self->lock);
        }
        return current_node;
    }
    int AStackPush(struct AStack* self,void* value){
        if(self->isLockSupported){
            sem_wait(&self->lock);
        }
        int status = 0;
        if(self->count<self->fixed_capacity){
            self->pointer[++(self->count)] = value;
            status = 1;
        }
        if(self->isLockSupported){
            sem_post(&self->lock);
        }
        return status;
    }
    int AStackPop(struct AStack* self){
        if(self->isLockSupported){
            sem_wait(&self->lock);
        }
        int status = 0;
        if(self->count>0){
            self->count--;
            status =  1;
        }
        if(self->isLockSupported){
            sem_post(&self->lock);
        }
        return status;
    }
    unsigned char AStackIsEmpty(struct AStack* self){
        unsigned char value=0;
        if(self->isLockSupported){
            sem_wait(&self->lock);
        }
        value =  self->count==0;
        if(self->isLockSupported){
            sem_post(&self->lock);
        }
        return value;
    }
    unsigned char AStackIsFull(struct AStack* self){
        if(self->isLockSupported){
            sem_wait(&self->lock);
        }
        unsigned char value =  self->count==self->fixed_capacity;
        if(self->isLockSupported){
            sem_post(&self->lock);
        }
        return value;
    }
    void AStackRelease(struct AStack* self){
        while(self->isEmpty(self)){
            self->pop(self);
        }
        if(self->isLockSupported){
            sem_destroy(&self->lock);
        }
    }
    unsigned int MinUnsignedInt(unsigned int a,unsigned int b){
        if(a<=b){
            return a;
        }
        else{
            return b;
        }
    }
    struct AStack* newAStackPointer(unsigned int concurrent_count,unsigned int fixed_capacity){
        struct AStack* array_stack = (struct AStack*)malloc(sizeof(struct AStack));
        memset(array_stack,0,sizeof(struct AStack));
        array_stack->isLockSupported = concurrent_count>0;
        if(array_stack->isLockSupported){
            sem_init(&array_stack->lock,0,concurrent_count);
        }
        array_stack->fixed_capacity = MinUnsignedInt((unsigned int)ARRAY_STACK_MAX_CAPACITY,fixed_capacity);
        array_stack->top = AStackTop;
        array_stack->push = AStackPush;
        array_stack->pop = AStackPop;
        array_stack->isEmpty = AStackIsEmpty;
        array_stack->isFull = AStackIsFull;
        array_stack->release = AStackRelease;
        return array_stack;
    }

    struct AStack newAStack(unsigned int concurrent_count,unsigned int fixed_capacity){
        struct AStack array_stack = (struct AStack){
            .isLockSupported = concurrent_count>0,
            .count = 0,
            .fixed_capacity = fixed_capacity,
            .top = AStackTop,
            .push = AStackPush,
            .pop = AStackPop,
            .isEmpty = AStackIsEmpty,
            .isFull = AStackIsFull,
            .release = AStackRelease
        };
        
        if(array_stack.isLockSupported){
            sem_init(&array_stack.lock,0,concurrent_count);
        }
        return array_stack;
    }
    