#include "array_queue.h"
unsigned int AQueue_getSize(struct AQueue* self){
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }
    unsigned int value = self->size;
    if(self->isLockSupported){
        sem_post(&self->lock);
    }
    return value;
}
unsigned int AQueue_isEmpty(struct AQueue* self){
   // printf("[AQueue_isEmpty] start\n");
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }
    unsigned int value = self->size==0;
    if(self->isLockSupported){
        sem_post(&self->lock);
    }
    //printf("[AQueue_isEmpty] finish");
    return value;
}
unsigned int AQueue_isFull(struct AQueue* self){
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }
    unsigned int value = self->size+1==self->fixed_capacity;
    if(self->isLockSupported){
        sem_post(&self->lock);
    }
    return value;
}
unsigned int getUnsignedIntMin(unsigned int a,unsigned int b){
    return a<b?a:b;
}
unsigned int getUnsignedIntMax(unsigned int a,unsigned int b){
    return a>b?a:b;
}
void AQueue_clear(struct AQueue* self){
    //printf("[AQueue_clear] start\n");
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }
    self->size = 0;
    self->current_pointer_index = 0;
    unsigned int min_count = getUnsignedIntMin(10,self->fixed_capacity);
    for(int i=0;i<min_count;i++){
        self->array_space[i] = NULL;
    }
    if(self->isLockSupported){
        sem_post(&self->lock);
    }
 //printf("[AQueue_clear] finish\n");
}
void AQueue_freeInternal(struct AQueue* self){

}
void* AQueue_front(struct AQueue* self){
    //printf("[AQueue_front] start\n");
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }
    void* value = self->array_space[(self->current_pointer_index+1)%(self->fixed_capacity)];
    if(self->isLockSupported){
        sem_post(&self->lock);
    }
    //printf("[AQueue_front] finish\n");

    return value;
}
void AQueue_pop(struct AQueue* self){
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }
    if(self->size>0){
        self->array_space[(self->current_pointer_index+1)%(self->fixed_capacity)] = NULL;
        self->current_pointer_index = (self->current_pointer_index+1)%(self->fixed_capacity);
        self->size--;
    }
    if(self->isLockSupported){
        sem_post(&self->lock);
    }
    
}
    
void AQueue_push(struct AQueue* self,void* value){
    //printf("[AQueue_push] push start\n");
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }
    if(self->size+1<self->fixed_capacity){
        self->size++;
        self->array_space[(self->current_pointer_index+self->size)%self->fixed_capacity] = value;
    }
    if(self->isLockSupported){
        sem_post(&self->lock);
    }
    //printf("[AQueue_push] push finish\n");
}
void AQueue_release(struct AQueue* self){

    self->clear(self);
    free(self->array_space);
    sem_destroy(&self->lock);
}
unsigned int findIndexPointer(struct AQueue* self){
    unsigned int st = 0;
    unsigned int ed = self->fixed_capacity-1;
    while(st<ed){
        unsigned int mid = (st+ed)/2;
        if(self->array_space[mid]==NULL){
            st = mid+1;
        }
        else{
            ed = mid-1;
        }
    }
    return st<ed?st:ed;
}
void AQueue_sort(struct AQueue* self){
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }
    if(self->size>0){
        printf("[AQueue_sort] start qsort\n");
        printf("[AQueue_sort]\t%x , %llu , (%d,%d) , %x\n",self->array_space,self->fixed_capacity,sizeof(void*),sizeof(struct AQueue*),self->comparator);
        qsort(self->array_space,self->fixed_capacity,sizeof(void*),self->comparator);
        printf("[AQueue_sort] finish qsort\n");
        unsigned int tmp_pointer = findIndexPointer(self);
        unsigned int accurate_pointer_index = tmp_pointer-1;
        unsigned char isNull = self->array_space[0]==NULL;
        if(isNull){
            unsigned int accurate_pointer_index = tmp_pointer-1;
            for(int i=tmp_pointer-2;i<tmp_pointer+3;i++){
                if(i<0){
                    continue;
                }
                if(isNull&&self->array_space[i]!=NULL){
                    int selected_index = (self->fixed_capacity+i-1)%self->fixed_capacity;
                    printf("[aqueue_sort] selected_index: %d\n",selected_index);
                    self->current_pointer_index = selected_index;
                    break;
                }
            }
        }
        else{
            self->current_pointer_index = self->fixed_capacity-1;
        }
    }
    if(self->isLockSupported){
        sem_post(&self->lock);
    }
}

struct AQueue* newAQueue(unsigned int fixed_capacity, int concurrent_mode,int (*comparator)(void*,void*)){
    //printf("[newAQueue] start init\n");
    struct AQueue* array_queue = (struct AQueue*)malloc(sizeof(struct AQueue));
    memset(array_queue,0,sizeof(struct AQueue));
    array_queue->isLockSupported = (concurrent_mode>0);
    array_queue->fixed_capacity = fixed_capacity+1;
    array_queue->array_space = (void**)malloc((array_queue->fixed_capacity+1)*sizeof(void*));
    memset(array_queue->array_space,0,(array_queue->fixed_capacity+1)*sizeof(void*));
    if(array_queue->isLockSupported){
        sem_init(&array_queue->lock,0,concurrent_mode);
    }
    array_queue->sort = AQueue_sort;
    array_queue->pop = AQueue_pop;
    array_queue->push = AQueue_push;
    array_queue->getSize = AQueue_getSize;
    array_queue->isEmpty = AQueue_isEmpty;
    array_queue->isFull = AQueue_isFull;
    array_queue->front = AQueue_front;
    array_queue->clear = AQueue_clear;
    array_queue->release = AQueue_release;
    array_queue->free = AQueue_release;
    array_queue->comparator = comparator;
//printf("[newAQueue] finish init\n");
    return array_queue;
}
