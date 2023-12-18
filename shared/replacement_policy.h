#pragma once
#ifndef __HEADER_REPLACEMENT_POLICY__
    #define __HEADER_REPLACEMENT_POLICY__
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <memory.h>
    #include <semaphore.h>
    #include <pthread.h>
    #include <time.h>
    #include "linked_queue.h"
    #include "doubly_linked_deque.h"
    #include "memory_buffer_structures.h"
    
    #define REPLACEMENT_POLICY_TYPE_FIFO 0
    #define REPLACEMENT_POLICY_TYPE_LFU 1
    #define REPLACEMENT_POLICY_TYPE_LRU 2
    struct FIFOPolicy{
        unsigned char isLockSupported:1;
        sem_t lock;
        struct DoublyLinkedDeque* deque;
        void (*updateAccessInfo)(struct FIFOPolicy* self,struct MemoryBuffer* hash_wrapper);
        void (*deleteAt)(struct FIFOPolicy* self,struct MemoryBuffer* hashmap_wrapper);
        struct DoublyLinkedDequeInternalNode* (*getEvictableNode)(struct FIFOPolicy* self);
    };
    struct LRUPolicy{
        unsigned char isLockSupported:1;
        sem_t lock;
        struct DoublyLinkedDeque* deque;
        void (*updateAccessInfo)(struct LRUPolicy* self,struct MemoryBuffer* hash_wrapper);
        void (*deleteAt)(struct LRUPolicy* self,struct MemoryBuffer* hashmap_wrapper);
        struct DoublyLinkedDequeInternalNode* (*getEvictableNode)(struct LRUPolicy* self);
    };
    struct LFUPolicy{
        unsigned char isLockSupported:1;
        sem_t lock;
        struct DoublyLinkedDeque* deque;
        void (*updateAccessInfo)(struct LFUPolicy* self,struct MemoryBuffer* hash_wrapper);
        void (*deleteAt)(struct LFUPolicy* self,struct MemoryBuffer* hashmap_wrapper);
        struct DoublyLinkedDequeInternalNode* (*getEvictableNode)(struct LFUPolicy* self);
    };
    struct ReplacementPolicyManager{
        struct FIFOPolicy* fifo;
        struct LRUPolicy* lru;
        struct LFUPolicy* lfu;
        unsigned char primary_policy_type;
        void (*resetCachePolicies)(struct ReplacementPolicyManager* self);
        void (*updateAccessInfo)(struct ReplacementPolicyManager* self,struct MemoryBuffer* hashmap_wrapper);
        pthread_t (*updateAccessInfoWithThread)(struct ReplacementPolicyManager* self,struct MemoryBuffer* hashmap_wrapper);
        void (*deleteAt)(struct ReplacementPolicyManager* self,struct MemoryBuffer* hashmap_wrapper);
        void (*debugPolicyContent)(struct ReplacementPolicyManager* self,int policy_type);
        /**
            policy_type
                0: fifo
                1: lfu
                2: lru
        */
        struct DoublyLinkedDequeInternalNode* (*getEvictableNode)(struct ReplacementPolicyManager* self,int policy_type);

    };

    

    struct FIFOPolicy* newFIFOPolicy(unsigned int concurrent_count);
    struct ReplacementPolicyManager* newReplacementPolicyManager(unsigned char primary_policy_type,unsigned int concurrent_count);
#endif