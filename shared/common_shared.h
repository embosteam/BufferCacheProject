#ifndef __HEADER_COMMON_SHARED__
    #define __HEADER_COMMON_SHARED__
    #include "md5.h"
    #include "hashmap.h"
    #include "linked_queue.h"
    #include "memory_buffer.h"
    #include "doubly_linked_list.h"
    /**
     * 디스크 관리하는 객체
    */
    struct DiskBufferWrapper{
        int disk_fd;
    };
#endif