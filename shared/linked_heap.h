#ifndef __HEADER_LINKED_HEAP__
    #define __HEADER_LINKED_HEAP__
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <memory.h>
    #include "linked_stack.h"
    #include "array_stack.h"
    struct LHeapInternalNode{
        struct LHeapInternalNode* left;
        struct LHeapInternalNode* right;
        struct LHeapInternalNode* parent;
        void* value;
        unsigned int index;
    };


    struct LHeap{
        unsigned char isLockSupported:1;
        sem_t lock;
        unsigned int count;
        struct LHeapInternalNode* root;

        int (*fetchKeyFromValue)(struct LHeap* self,void* value);
        int (*comparator)(struct LHeap* self,void* current_value,void* parent_value);
        int (*push)(struct LHeap* self,void* value);
        unsigned int (*getCount)(struct LHeap* self);
        void* (*get)(struct LHeap* self);
        unsigned char (*clear)(struct LHeap*);
        unsigned char (*isEmpty)(struct LHeap*);

        unsigned char (*pop)(struct LHeap* self);
        void (*release)(struct LHeap* self);
    };
    
    struct LHeap* newLHeap(unsigned int concurrent_count,int (*fetchKeyFromValue)(struct LHeap*,void*),int (*comparator)(struct LHeap*,void*,void*));
    struct LHeap* newLHeap2(unsigned int concurrent_count,int (*fetchKeyFromValue)(struct LHeap*,void*));
#endif