#pragma once
#ifndef __HEADER_LINKED_STACK__
    #define __HEADER_LINKED_STACK__
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <memory.h>
    #include <semaphore.h>
    struct LStackInternalNode{
        struct LStackInternalNode* next;
        struct LStackInternalNode* previous;
        void* value;
    };
    struct LStack{
        unsigned char isLockSupported:1;
        sem_t lock;
        unsigned int count;
        unsigned int fixed_capacity;
        
        
        struct LStack* pointer;
        void* (*top)(struct LStack* self);
        int (*push)(struct LStack* self,void* value);
        int (*pop)(struct LStack* self);
        
        unsigned char (*isEmpty)(struct LStack* self);
        unsigned char (*isFull)(struct LStack* self);
        void (*release)(struct LStack* self);
    };
    struct LStack* newLStack(unsigned int concurrent_count,unsigned int fixed_capacity);
    
#endif