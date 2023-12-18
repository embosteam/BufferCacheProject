//#pragma once
#ifndef __HEADER_ARRAY_QUEUE__
    #define __HEADER_ARRAY_QUEUE__
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <semaphore.h>
    #include <memory.h>
    
    
    struct AQueue{
        unsigned char isLockSupported:1;
        sem_t lock;
        unsigned int size;
        
        unsigned int fixed_capacity;

        
        void* *array_space;
        int current_pointer_index;
        void* (*front)(struct AQueue*);
        void (*pop)(struct AQueue*);
        unsigned int (*getSize)(struct AQueue*);
        unsigned int (*isEmpty)(struct AQueue*);
        unsigned int (*isFull)(struct AQueue*);

        void (*push)(struct AQueue*,void*);
        void (*clear)(struct AQueue*);
        void (*free)(struct AQueue*);
        void (*release)(struct AQueue*);
        void (*sort)(struct AQueue*);
        
        int (*comparator)(void* s1,void* s2);//s1이크면 1, s2가 크면 -1, 그외 0을 반환하도록 구현
    };
    struct AQueue* newAQueue(unsigned int fixed_capacity, int concurrent_mode,int (*comparator)(void*,void*));
   
    
#endif