#ifndef __DISK_WRITE_HEADER__
    #define __DISK_WRITE_HEADER__
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "../shared/memory_buffer_structures.h"
    #include "../shared/linked_queue.h"
    #include "flush_thread.h"
    #include "../shared/time_utils.h"
    #include "../config.h"
    /**
     * thread를 내부적으로 쓰레드를 사용하지 않으며
     * disk_fd위치를 dup()함수로 복사해서 memory_buffer의 실제 내용(char*)을 디스크에 쓰는 함수
    */
    int write2Disk(int disk_fd,struct MemoryBuffer* memory_buffer);
    int write2DiskMultiple(int disk_fd,struct MemoryBuffer** mem_buffers,const int mem_buffer_count);

    int write2Disk2(char* disk_buffer,struct MemoryBuffer* memory_buffer);
    int write2DiskMultiple2(char* disk_buffer,struct MemoryBuffer** mem_buffers,const int mem_buffer_count);
    /**
     * 일종의 wrapper함수로 내부적으로 위쪽의 int write2Disk(int disk_fd,struct MemoryBuffer* memory_buffer); 를 호출함
     * 이때 내부에서 함수 파라매터 args를 
     * struct <Parmas>{
     *  pthread_attr_t attr;
     *  int disk_fd;
     *  struct MemoryBuffer* memory_buffer;
     * }
     * 로 변환해 write2Disk를 호출함
    */
    int write2DiskInThread(void* args);
    int write2DiskMultipleInThread(void* args);

    int write2DiskInThread2(void* args);
    int write2DiskMultipleInThread2(void* args);
    
#endif