#pragma once
#ifndef __HEADER_MEMORY_BUFFER_STRUCTURES__
    #define __HEADER_MEMORY_BUFFER_STRUCTURES__
    #include <semaphore.h>
    #include <pthread.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <unistd.h>
    #include <stdint.h>
    #include "doubly_linked_deque.h"
    /**
     * 각 버퍼에 붙는 메모리 버퍼 헤더
    */
    struct MemoryBufferHeader{
        unsigned char isValid:1;
        unsigned char isDirty:1;
        unsigned char isBufferBeingWrittenNow:1;//데이터 동기화주기 약간불안정, sem_getvalue 함수등을 사용해 체크하는게 조금더 바람직
        int request_write_count;
        //pthread_t writeThreadId;
        
        sem_t write_lock;
        long block_number;
        long long block_size_byte;
        uint8_t buffer_precalculated_hash[16];
        long lastestAccessedTimeStamp;

        unsigned int access_count;
        unsigned long long lastest_accessing_timestamp;
        struct DoublyLinkedDequeInternalNode* lru_node;
        struct DoublyLinkedDequeInternalNode* fifo_node;
        struct DoublyLinkedDequeInternalNode* lfu_node;
    };


    /**
     * 실제 버퍼 내용과 헤더를 합쳐놓은 메모리 버퍼의 실질적인 개체
    */
    struct MemoryBuffer{
        struct MemoryBufferHeader header;
        char* buffer;
    };
    
    struct HashMapWrapper{
        int block_number;
        struct MemoryBuffer* memory_buffer;
        /*unsigned int access_count;
        unsigned long long lastest_accessing_timestamp;
        struct DoublyLinkedDequeInternalNode* lru_node;
        struct DoublyLinkedDequeInternalNode* fifo_node;
        struct DoublyLinkedDequeInternalNode* mfu_node;*/
    };
#endif