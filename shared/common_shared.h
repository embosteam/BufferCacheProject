#ifndef __HEADER_COMMON_SHARED__
    #define __HEADER_COMMON_SHARED__
    #include "md5.h"
    #include "hashmap.h"
    #include "linked_heap.h"
    #include "linked_queue.h"
    #include "linked_stack.h"
    
    #include "array_stack.h"
    #include "memory_buffer.h"
    #include "doubly_linked_list.h"
    #include "random_utils.h"
    #include "time_utils.h"
    /**
     * 디스크 관리하는 객체
    */
    struct DiskBufferWrapper{
        int disk_fd;
    };
#endif