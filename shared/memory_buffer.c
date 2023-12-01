#include "memory_buffer.h"
struct MemoryBuffer* createNewMemoryBuffer(int block_size,int block_number){
        struct MemoryBuffer* buffer = NULL;
        buffer = (struct MemoryBuffer*)malloc(sizeof(struct MemoryBuffer));
        memset(buffer,0,sizeof(struct MemoryBuffer));
        buffer->buffer = (char*)malloc(block_size*1024);//aligned_alloc(block_size,block_size*sizeof(char));
        buffer->header.block_number = block_number;
        buffer->header.block_size_byte = block_size*1024;
        printf("[createNewMemoryBuffer] isBufferNULL: %d\n",buffer->buffer==NULL);
        sem_init(&buffer->header.write_lock,0,1);
        return buffer;
    }
    /**
     * 
     * 새로운 메모리 버퍼 관리 객체를 생성하는 함수
     *      n: 디스크의 버퍼의 총 갯수
     *      manageable_n: 4KB 버퍼가 만약 n=10000개이상이 되면, 실질적으로 RAM에 탑재가 불가능하므로, RAM에 올릴 수 있는 버퍼의 최대 갯수
     *      block_size: 각 메모리 버퍼가 가지는 실질적인 크기(단위는 바이트(B))
     */
    struct MemoryBufferManager* createNewMemoryBufferManager(int n,int manageable_n,long long block_size){
        if(manageable_n>n){
            perror("error: manageable_n>n");
            return NULL;
        }
        struct MemoryBufferManager* manager = NULL;
        manager = (struct MemoryBufferManager*)malloc(sizeof(struct MemoryBufferManager));
        memset(manager,0,sizeof(struct MemoryBufferManager));
        manager->explicit_block_size=block_size;
        manager->total_number_of_memory_buffers = n;
        manager->manageable_number_of_memory_buffers=manageable_n;
        manager->memory_buffers = (struct MemoryBufferManager**)malloc(manageable_n*sizeof(struct MemoryBuffer*));
        return manager;
    }
    void freeMemoryBufferAt(struct MemoryBufferManager* memory_buffer_wrapper,int index){
        if(0<=index&&index<memory_buffer_wrapper->manageable_number_of_memory_buffers){
            struct MemoryBuffer* buffer = memory_buffer_wrapper->memory_buffers[index];
            free(buffer->buffer);
            free(memory_buffer_wrapper->memory_buffers[index]);
            sem_destroy(&buffer->header.write_lock);
            memory_buffer_wrapper->memory_buffers[index] = NULL;
        }
    }
    void allFreeMemoryBuffer(struct MemoryBufferManager* memory_buffer_wrapper){
        for(int i=0;i<memory_buffer_wrapper->manageable_number_of_memory_buffers;i++){
            freeMemoryBufferAt(memory_buffer_wrapper,i);
        }
    }
    int setMemoryBufferAt(struct MemoryBufferManager* memory_buffer_wrapper,int index,char* buffer){
        if(0<=index&&index<memory_buffer_wrapper->manageable_number_of_memory_buffers){
            if(memory_buffer_wrapper->memory_buffers[index]==NULL){
                memory_buffer_wrapper->memory_buffers[index] = createNewMemoryBuffer(memory_buffer_wrapper->explicit_block_size,-1);
            }
            memcpy(memory_buffer_wrapper->memory_buffers[index]->buffer,buffer,memory_buffer_wrapper->explicit_block_size*sizeof(char));
            return 1;
        }
        return 0;
    }