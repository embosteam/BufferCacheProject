#include "buffer_write.h"
/**
 * 매개변수 임시 할당을 위한 구조체
*/
struct WriteParametersInThread{
    pthread_attr_t attr;
    int disk_fd;
    struct MemoryBuffer* memory_buffer;
};
/**
     * 버퍼를 파라메터에서 넘어온 값들을 기준으로 구현한 버퍼 저장 구조체에 넣어줌
     * 리턴값: buffer_content의 크기
    */
int writeToBuffer(int disk_fd,struct MemoryBuffer* memory_buffer){
    (memory_buffer->header.isDirty) = 1;
    //TODO: save block to buffer structure
    

    int ret=0;
    struct MemoryBufferHeader memory_buffer_header = memory_buffer->header;
    const int block_number = memory_buffer_header.block_number;
    const char* buffer_content = memory_buffer->buffer;
    const int block_size = memory_buffer_header.block_size_byte;
    //ret = write2Disk(disk_fd,memory_buffer);
   struct WriteParametersInThread params = {
    .disk_fd = disk_fd,
    .memory_buffer = memory_buffer
   };
    pthread_t tid = createAndRunFlushThread(&write2DiskInThread,(void*)&params);
    //memory_buffer->header.writeThreadId = tid;
    return ret;
}
