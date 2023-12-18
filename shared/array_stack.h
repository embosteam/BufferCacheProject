#pragma once
#ifndef __HEADER_ARRAY_STACK__
    #define __HEADER_ARRAY_STACK__
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <memory.h>
    #include <math.h>
    #include <semaphore.h>
    #define ARRAY_STACK_MAX_CAPACITY 1024
    struct AStack{
        unsigned char isLockSupported:1;
        sem_t lock;
        unsigned int count;
        unsigned int fixed_capacity;
        
        
        void* pointer[ARRAY_STACK_MAX_CAPACITY];
        void* (*top)(struct AStack* self);
        int (*push)(struct AStack* self,void* value);
        int (*pop)(struct AStack* self);
        
        unsigned char (*isEmpty)(struct AStack* self);
        unsigned char (*isFull)(struct AStack* self);
        void (*release)(struct AStack* self);
    };
    struct AStack* newAStackPointer(unsigned int concurrent_count,unsigned int fixed_capacity);
    struct AStack newAStack(unsigned int concurrent_count,unsigned int fixed_capacity);
#endif