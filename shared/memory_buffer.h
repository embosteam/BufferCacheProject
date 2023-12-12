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
    
    /**
     * 각 버퍼에 붙는 메모리 버퍼 헤더
    */
    struct MemoryBufferHeader{
        unsigned char isValid:1;
        unsigned char isDirty:1;
        unsigned char isBufferBeingWrittenNow:1;//데이터 동기화주기 약간불안정, sem_getvalue 함수등을 사용해 체크하는게 조금더 바람직
        pthread_t writeThreadId;
        
        sem_t write_lock;
        long block_number;
        long long block_size_byte;
        uint8_t buffer_precalculated_hash[16];
        long lastestAccessedTimeStamp;
    };


    /**
     * 실제 버퍼 내용과 헤더를 합쳐놓은 메모리 버퍼의 실질적인 개체
    */
    struct MemoryBuffer{
        struct MemoryBufferHeader header;
        char* buffer;
    };

    struct MemoryBufferController{
        struct MD5Controller* hash_controller;

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
    
    struct MemoryBufferController* newMemoryBufferController();

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
    struct MemoryBufferManager* createNewMemoryBufferManager(int n,int manageable_n,long long block_size);
    //index에 위치한 메모리 버퍼 객체를 메모리 버퍼 메니저에서 리소스를 시스템에 반환하는 함수
    void freeMemoryBufferAt(struct MemoryBufferManager* memory_buffer_wrapper,int block_number,unsigned char free_content);
    //모든 메모리 버퍼 객체를 메모리 버퍼 메니저에서 리소스를 시스템에 반환하는 함수
    void allFreeMemoryBuffer(struct MemoryBufferManager* memory_buffer_wrapper);
    //index에 위치한 메모리 버퍼 객체의 내용을 적용하는 함수
    int setMemoryBufferAt(struct MemoryBufferManager* memory_buffer_wrapper,int block_number,char* buffer,long long buffer_length_byte);
    int putMemoryBufferAt(struct MemoryBufferManager* memory_buffer_wrapper,int block_number,struct MemoryBuffer* mem_buffer);
    struct MemoryBuffer*  getMemoryBufferMap(struct MemoryBufferManager* memory_buffer_wrapper,int);
#endif  