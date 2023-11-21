#ifndef __HEADER_COMMON_SHARED__
    #define __HEADER_COMMON_SHARED__
    struct MemoryBufferWrapper{
        int number_of_memory_buffers;
        int explicit_block_size;
        char** memory_buffers;
        int (* readMemoryBuffer)(int);
        void (* writeMemoryBuffer)(int,char*);
    };
    struct DiskBufferWrapper{
        int disk_fd;
    };
#endif