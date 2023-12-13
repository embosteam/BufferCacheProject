#include "buffered_read.h"

int isBufferEmpty(struct MemoryBufferManager* memory_buffer_wrapper)  // buffer가 비어있는지 확인한다.
{
    if(memory_buffer_wrapper->current_memory_buffer_count==0)
        return 1;
    else
        return memory_buffer_wrapper->total_number_of_memory_buffers;
}

int isBufferFull(struct MemoryBufferManager* memory_buffer_wrapper)   // buffer가 가득 차있는지 확인한다.
{
    if(memory_buffer_wrapper->current_memory_buffer_count==memory_buffer_wrapper->manageable_number_of_memory_buffers)
        return 1;
    else
        return 0;
}

int getBufferbyContent(struct MemoryBuffer* mem_buffer, char* content);  // content에 해당하는 블록을 찾고 없다면,disk에서 불러오고 아니면 바로 읽어서 보낸다.
{
    struct MemoryBufferController* memory_buffer_controller = memory_buffer_manager->controller;
    memory_buffer_controller.
}
int readFromDisk(int disk_fd, struct MemoryBuffer* mem_buffer, char* content);    // buffer가 다 찼으면 victim을 선정해서 비우고, 그렇지 않으면 읽어와서 넣는다.

int getVictim(struct MemoryBuffer* mem_buffer);  // victim을 선정한다.

