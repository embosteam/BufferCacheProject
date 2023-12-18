#ifndef __MEMORY_BUFFER__
#define __MEMORY_BUFFER__
    #include <stdlib.h>
    #include <stdio.h>
    #include <unistd.h>
    #include <memory.h>
    #include <semaphore.h>
    #include <stdint.h>
    #include "hash_controller.h"
    #include "hashmap.h"
    #include "doubly_linked_deque.h"
    #include "memory_buffer_structures.h"
    #include "replacement_policy.h"
    #include "../delayed_write/thread_pool.h"
    #include "time_utils.h"
    #include "../config.h"
    
    

    struct MemoryBufferController{
        struct MD5Controller* hash_controller;
        struct ReplacementPolicyManager* replacement_policy_manager;
        int (* readMemoryBuffer)(struct MemoryBufferController*,int);
        void (* writeMemoryBuffer)(struct MemoryBufferController*,int,char*);
        unsigned char (* checkContentValidation)(struct MemoryBufferController*,struct MemoryBuffer*,char*);
        void (* checkContentValidationAndSetValidBit)(struct MemoryBufferController*,struct MemoryBuffer*,char*);
        struct MemoryBuffer* (* getMemoryBufferMap)(struct MemoryBufferManager*,int);
        void (*freeMemoryBufferAt)(struct MemoryBufferManager* memory_buffer_wrapper,int block_number,unsigned char free_content);
        void (*allFreeMemoryBuffer)(struct MemoryBufferManager* memory_buffer_wrapper);
        int (*putMemoryBufferAt)(struct MemoryBufferManager* memory_buffer_wrapper,int block_number,struct MemoryBuffer* mem_buffer);
        int (*setMemoryBufferAt)(struct MemoryBufferManager* memory_buffer_wrapper,int block_number,char* buffer,long long buffer_length_byte);
        
        //policy 관련 함수
        int (*shouldFindEvictableBuffer)(struct MemoryBufferController* self,struct MemoryBufferManager* manager);
        struct MemoryBuffer* (*findEvictableBuffer)(struct MemoryBufferController* self,struct MemoryBufferManager* manager);
        int (*deleteMemoryBuffer)(struct MemoryBufferController* self,struct MemoryBufferManager* manager,struct MemoryBuffer* mem_buffer);
    };
    
    struct MemoryBufferController* newMemoryBufferController(unsigned char replacement_type);

    /**
     * 
    */
    struct MemoryBufferManager{
        int total_number_of_memory_buffers; 
        int manageable_number_of_memory_buffers;
        long long explicit_block_size;
        unsigned int current_memory_buffer_count;
        struct hashmap* memory_buffers;
        struct MemoryBufferController* controller;
        double pure_hashmap_get_time;
        double pure_hashmap_set_time;
        int disk_fd;
        char* disk_buffer;
        struct ThreadPool* tp;
        //char** memory_buffers;
        
    };

    


    //새로운 메모리 버퍼 객체를 생성하는 함수
    struct MemoryBuffer* createNewMemoryBuffer(int block_size,int block_number);
    /**
     * 
     * 새로운 메모리 버퍼 관리 객체를 생성하는 함수
     *      n: 디스크의 버퍼의 총 갯수
     *      manageable_n: 4KB 버퍼가 만약 n=10000개이상이 되면, 실질적으로 RAM에 탑재가 불가능하므로, RAM에 올릴 수 있는 버퍼의 최대 갯수
     *      block_size: 각 메모리 버퍼가 가지는 실질적인 크기(단위는 바이트(B))
     */
    struct MemoryBufferManager* createNewMemoryBufferManager(int n,int manageable_n,long long block_size,unsigned char replacement_type);
    //index에 위치한 메모리 버퍼 객체를 메모리 버퍼 메니저에서 리소스를 시스템에 반환하는 함수
    void freeMemoryBufferAt(struct MemoryBufferManager* memory_buffer_wrapper,int block_number,unsigned char free_content);
    //모든 메모리 버퍼 객체를 메모리 버퍼 메니저에서 리소스를 시스템에 반환하는 함수
    void allFreeMemoryBuffer(struct MemoryBufferManager* memory_buffer_wrapper);
    //index에 위치한 메모리 버퍼 객체의 내용을 적용하는 함수
    int setMemoryBufferAt(struct MemoryBufferManager* memory_buffer_wrapper,int block_number,char* buffer,long long buffer_length_byte);
    int putMemoryBufferAt(struct MemoryBufferManager* memory_buffer_wrapper,int block_number,struct MemoryBuffer* mem_buffer);
    struct MemoryBuffer*  getMemoryBufferMap(struct MemoryBufferManager* memory_buffer_wrapper,int);
#endif  