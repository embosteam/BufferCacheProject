#ifndef __MEMORY_BUFFER__
#define __MEMORY_BUFFER__
    #include <stdlib.h>
    #include <stdio.h>
    #include <unistd.h>
    #include <memory.h>
    struct MemoryBufferHeader{
        unsigned char isValid:1;
        unsigned char isDirty:1;
        long block_number;
        long lastestAccessedTimeStamp;
    };
    struct MemoryBuffer{
        struct MemoryBufferHeader header;
        char* buffer;
    };
    struct MemoryBufferManager{
        int total_number_of_memory_buffers;
        int manageable_number_of_memory_buffers;
        long long explicit_block_size;
        
        struct MemoryBuffer** memory_buffers;
        //char** memory_buffers;
        int (* readMemoryBuffer)(int);
        void (* writeMemoryBuffer)(int,char*);
    };
    struct MemoryBuffer* createNewMemoryBuffer(int block_size,int block_number);
    struct MemoryBufferManager* createNewMemoryBufferManager(int n,int manageable_n,long long block_size);
    void freeMemoryBufferAt(struct MemoryBufferManager* memory_buffer_wrapper,int index);
    void allFreeMemoryBuffer(struct MemoryBufferManager* memory_buffer_wrapper);
    int setMemoryBufferAt(struct MemoryBufferManager* memory_buffer_wrapper,int index,char* buffer);
    
#endif  