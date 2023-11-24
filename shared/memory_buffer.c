#include "memory_buffer.h"
struct MemoryBuffer* createNewMemoryBuffer(int block_size,int block_number){
        struct MemoryBuffer* buffer = NULL;
        buffer = (struct MemoryBuffer*)malloc(sizeof(struct MemoryBuffer));
        memset(buffer,0,sizeof(struct MemoryBuffer));
        buffer->buffer = (char*)aligned_alloc(block_size,block_size*sizeof(char));
        return buffer;
    }
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