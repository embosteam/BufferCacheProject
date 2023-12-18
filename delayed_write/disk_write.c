#include "disk_write.h"
static sem_t* disk_write_lock=NULL;
/**
     * thread를 내부적으로 쓰레드를 사용하지 않으며
     * pwrite함수로 memory_buffer의 실제 내용(char*)을 디스크에 쓰는 함수
    */
int write2Disk(int disk_fd,struct MemoryBuffer* memory_buffer){
    int ret;
    const unsigned long long start_time = getCurrentTime();

    struct MemoryBufferHeader* memory_buffer_header = &memory_buffer->header;
    if(memory_buffer_header->isValid==0){
        memory_buffer_header->isBufferBeingWrittenNow = 0;
        memory_buffer->header.request_write_count--;
        return (ret=0);
    }
    const int block_number = memory_buffer_header->block_number;
    const char* buffer_content = memory_buffer->buffer;
    const int block_size = memory_buffer_header->block_size_byte;
    int disk_fd2 = disk_fd;
    if(disk_fd2==-1){
        perror("\t  error: ");
        return -1;
    }
    sem_wait(&memory_buffer->header.write_lock);
    if(memory_buffer->header.request_write_count){
    memory_buffer->header.request_write_count--;
    }
    if(!memory_buffer->header.isDirty||!memory_buffer->header.isValid){
        memory_buffer_header->isBufferBeingWrittenNow=0;
        sem_post(&memory_buffer->header.write_lock);
        return -2;
    }
    memory_buffer->header.isBufferBeingWrittenNow = 1;
    const unsigned long long start_write_time = getCurrentTime();
    
    long long offset = block_number*block_size;
    ret = pwrite(disk_fd2,buffer_content,block_size,offset);
    if(ret<0){
        memory_buffer_header->isBufferBeingWrittenNow=0;
        sem_post(&memory_buffer->header.write_lock);
        perror("[write2Disk]file write error");
        return ret;
    }
    fdatasync(disk_fd2);
    //fsync(disk_fd2);
    const unsigned long long end_write_time = getCurrentTime();
    const unsigned long long real_write_time = (end_write_time-start_write_time);
    
    //close(disk_fd2);
    memory_buffer_header->isDirty = 0;
    memory_buffer_header->isBufferBeingWrittenNow = 0;
    sem_post(&memory_buffer->header.write_lock);
    const unsigned long long end_time = getCurrentTime();
    unsigned long long total_time = (end_time-start_time);
    printf("[L:DW:S:TT]%llu\n",total_time);
    printf("[L:DW:S:RW]%llu\n",real_write_time);
    return ret;
}
/**
* _mem_buffers_count 만큼의 버퍼들의 내용들을 하나의 버퍼로 이어붙여서 첫번쨰 버퍼의 offset에서
* 한번에 write해주는 함수
*/
int write2DiskMultiple(int disk_fd,struct MemoryBuffer** mem_buffers,const int _mem_buffers_count){
    int ret;
    const int mem_buffers_count = _mem_buffers_count;
    const unsigned long long start_time = getCurrentTime();
    struct MemoryBuffer** buffers_pointer = mem_buffers;
    unsigned long long merged_byte_size = 0;
    for(int i=0;i<mem_buffers_count;i++){
        merged_byte_size+=mem_buffers[i]->header.block_size_byte;
    }
    char* new_buffer = (char*)aligned_alloc(sizeof(char)*merged_byte_size+1,sizeof(char)*merged_byte_size+1);
    unsigned long long offset = 0;
     buffers_pointer = mem_buffers;
    struct MemoryBuffer* first_mem_buffer = mem_buffers[0];
    long long start_block_number = first_mem_buffer->header.block_number;
    long long default_block_size_byte = first_mem_buffer->header.block_size_byte;
    long long disk_offset = start_block_number*default_block_size_byte;
    int disk_fd2 =disk_fd;// dup(disk_fd);
    if(disk_fd2==-1){
        //sem_post(disk_write_lock);
        perror("\t  error: ");
        free(new_buffer);
        return -1;
    }
    for(int i=0;i<mem_buffers_count;i++){
        unsigned dummy1=0;
            unsigned dummy2 =0;
        sem_wait(&((buffers_pointer[i])->header.write_lock));
        if(buffers_pointer[i]->header.request_write_count){
            buffers_pointer[i]->header.request_write_count--;
        }
        unsigned dummy3=0;
            unsigned dummy4 =0;
        (&buffers_pointer[i]->header)->isBufferBeingWrittenNow = 1;
        
        unsigned long long block_size = (buffers_pointer[i])->header.block_size_byte;
        if(CONFIG_DEBUG_MODE){
        printf("  [write2DiskMultiple%d](i=%d) buffer_cnt:%lld offset=%lld , block_size= %llu(%x)\n",start_block_number,i,mem_buffers_count,offset,block_size,block_size);
        printf("\t[write2DiskMultiple%d](i=%d) pointer: %x\n",start_block_number,i,new_buffer+offset);
        printf("\t[write2DiskMultiple%d](i=%d) buffer_valid=%d buffer is NULL(%d)\n",start_block_number,i,(buffers_pointer[i])->header.isValid,(buffers_pointer[i])->buffer==NULL);
        printf("\t[write2DiskMultiple%d](i=%d) new buffer_range: %x ~ %x\n",start_block_number,i,new_buffer,new_buffer+merged_byte_size);
        printf("\t[write2DiskMultiple%d](i=%d) new buffer writing_range: %x ~ %x\n",start_block_number,i,new_buffer+offset,new_buffer+offset+block_size);
        }
        char* tmp_buffer = (char*)malloc(sizeof(char)*block_size);
        memcpy(tmp_buffer,(buffers_pointer[i])->buffer,block_size);
        memcpy(new_buffer+offset,tmp_buffer,block_size);
        free(tmp_buffer);
        offset +=block_size;
    }
    const unsigned long long start_write_time = getCurrentTime();
    ret = pwrite(disk_fd2,new_buffer,merged_byte_size,disk_offset);
    
    if(ret<0){
        for(int i=0;i<mem_buffers_count;i++){
            unsigned dummy1=0;
            unsigned dummy2 =0;
            (&buffers_pointer[i]->header)->isBufferBeingWrittenNow = 0;
            sem_post(&((buffers_pointer[i])->header.write_lock));
        }
        free(new_buffer);
        perror("[write2DiskMultiple]file write error");
        return ret;
    }
    fdatasync(disk_fd2);
    //fsync(disk_fd2);
    const unsigned long long end_write_time = getCurrentTime();
    const unsigned long long real_write_time = (end_write_time-start_write_time);
    
    
    for(int i=0;i<mem_buffers_count;i++){
        unsigned dummy1=0;
        unsigned dummy2 =0;
       ( &buffers_pointer[i]->header)->isDirty = 0;
        (&buffers_pointer[i]->header)->isBufferBeingWrittenNow = 0;
        sem_post(&buffers_pointer[i]->header.write_lock);
        
    }
    free(new_buffer);
    free(mem_buffers);
    buffers_pointer = NULL;
    mem_buffers=NULL;
    printf("[write2DiskMultiple] finished %d (cnt: %d)\n",start_block_number,mem_buffers_count);
    
    const unsigned long long end_time = getCurrentTime();
    unsigned long long total_time = (end_time-start_time);
    printf("[L:DW:M:TT]%llu\n",total_time);
    printf("[L:DW:M:RW]%llu\n",real_write_time);
    return ret;
}

/**
     * mmap영역에 
     * memcpy를 적용하고 msync로 동기화해주는 함수
    */
int write2Disk2(char* disk_buffer,struct MemoryBuffer* memory_buffer){
    int ret;
    const unsigned long long start_time = getCurrentTime();
    struct MemoryBufferHeader* memory_buffer_header = &memory_buffer->header;
    if(memory_buffer_header->isValid==0){
        memory_buffer_header->isBufferBeingWrittenNow = 0;
        return (ret=0);
    }
    const int block_number = memory_buffer_header->block_number;
    const char* buffer_content = memory_buffer->buffer;
    const int block_size = memory_buffer_header->block_size_byte;

    sem_wait(&memory_buffer->header.write_lock);

    if(!memory_buffer->header.isDirty||!memory_buffer->header.isValid){
        printf("[write2Disk2] ( blk_nr:%d) unlocked due to invalidation\n",
        memory_buffer->header.block_number);
    
        memory_buffer_header->isBufferBeingWrittenNow=0;
        sem_post(&memory_buffer->header.write_lock);
        return -2;
    }
    memory_buffer->header.isBufferBeingWrittenNow = 1;
    if(memory_buffer->header.request_write_count>0){
        memory_buffer->header.request_write_count--;
    }
    const unsigned long long start_write_time = getCurrentTime();
    
    long long offset = block_number*block_size;
    memcpy(disk_buffer+offset,memory_buffer->buffer,block_size);
    msync(disk_buffer+offset,block_size,4);
    const unsigned long long end_write_time = getCurrentTime();
    const unsigned long long real_write_time = (end_write_time-start_write_time);
    
    memory_buffer->header.isDirty = 0;
    memory_buffer->header.isBufferBeingWrittenNow = 0;
    sem_post(&memory_buffer->header.write_lock);
    
    const unsigned long long end_time = getCurrentTime();
    unsigned long long total_time = (end_time-start_time);
    printf("[L:DW:S:TT]%llu\n",total_time);
    printf("[L:DW:S:RW]%llu\n",real_write_time);
    return ret;
}
/**
     * _mem_buffers_count갯수 만큼의 연속적인 버퍼 내용을 이어붙여서 mmap영역에 
     * memcpy를 적용하고 msync로 동기화해주는 함수
    */
int write2DiskMultiple2(char* disk_buffer,struct MemoryBuffer** mem_buffers,const int _mem_buffers_count){
    int ret;
    const int mem_buffers_count = _mem_buffers_count;
    const unsigned long long start_time = getCurrentTime();
    struct MemoryBuffer** buffers_pointer = mem_buffers;
    unsigned long long merged_byte_size = 0;
    for(int i=0;i<mem_buffers_count;i++){
        merged_byte_size+=mem_buffers[i]->header.block_size_byte;
    }
    char* new_buffer = (char*)aligned_alloc(sizeof(char)*merged_byte_size+1,sizeof(char)*merged_byte_size+1);
    unsigned long long offset = 0;
     buffers_pointer = mem_buffers;
    struct MemoryBuffer* first_mem_buffer = mem_buffers[0];
    long long start_block_number = first_mem_buffer->header.block_number;
    long long default_block_size_byte = first_mem_buffer->header.block_size_byte;
    long long disk_offset = start_block_number*default_block_size_byte;
    for(int i=0;i<mem_buffers_count;i++){
        sem_wait(&((buffers_pointer[i])->header.write_lock));
        unsigned dummy3=0;
            unsigned dummy4 =0;
        if( (&buffers_pointer[i]->header)->request_write_count>0){
            
        (&buffers_pointer[i]->header)->request_write_count -= 1;
        }
        (&buffers_pointer[i]->header)->isBufferBeingWrittenNow = 1;
    }
    for(int i=0;i<mem_buffers_count;i++){
        
        unsigned long long block_size = (buffers_pointer[i])->header.block_size_byte;
        if(CONFIG_DEBUG_MODE){
        printf("  [write2DiskMultiple%d](i=%d) buffer_cnt:%lld offset=%lld , block_size= %llu(%x)\n",start_block_number,i,mem_buffers_count,offset,block_size,block_size);
        printf("\t[write2DiskMultiple%d](i=%d) pointer: %x\n",start_block_number,i,new_buffer+offset);
        printf("\t[write2DiskMultiple%d](i=%d) buffer_valid=%d buffer is NULL(%d)\n",start_block_number,i,(buffers_pointer[i])->header.isValid,(buffers_pointer[i])->buffer==NULL);
        printf("\t[write2DiskMultiple%d](i=%d) new buffer_range: %x ~ %x\n",start_block_number,i,new_buffer,new_buffer+merged_byte_size);
        printf("\t[write2DiskMultiple%d](i=%d) new buffer writing_range: %x ~ %x\n",start_block_number,i,new_buffer+offset,new_buffer+offset+block_size);
        }
        char* tmp_buffer = (char*)aligned_alloc(block_size,block_size);
        memcpy(tmp_buffer,(buffers_pointer[i])->buffer,block_size);
        memmove(new_buffer+offset,tmp_buffer,block_size);
        free(tmp_buffer);
        offset +=block_size;
    }
    const unsigned long long start_write_time = getCurrentTime();
    memcpy(disk_buffer+disk_offset,new_buffer,merged_byte_size);
    msync(disk_buffer+disk_offset,merged_byte_size,4);
    //fdatasync(disk_fd2);
    const unsigned long long end_write_time = getCurrentTime();
    const unsigned long long real_write_time = (end_write_time-start_write_time);
    
    
    for(int i=0;i<mem_buffers_count;i++){
        unsigned dummy1=0;
        unsigned dummy2 =0;
       ( &buffers_pointer[i]->header)->isDirty = 0;
        (&buffers_pointer[i]->header)->isBufferBeingWrittenNow = 0;
        sem_post(&buffers_pointer[i]->header.write_lock);
        
    }
    free(new_buffer);
    free(mem_buffers);
    buffers_pointer = NULL;
    mem_buffers=NULL;
    printf("[write2DiskMultiple2] finished %d (cnt: %d)\n",start_block_number,mem_buffers_count);
    
    const unsigned long long end_time = getCurrentTime();
    unsigned long long total_time = (end_time-start_time);
    printf("[L:DW:M:TT]%llu\n",total_time);
    printf("[L:DW:M:RW]%llu\n",real_write_time);
    return ret;
}


/**
 * 매개변수 임시 할당을 위한 구조체
*/
struct WriteParametersInThread{
    pthread_attr_t attr;
    int disk_fd;
    struct MemoryBuffer* memory_buffer;
};
struct MultiWriteParametersInThread{
    pthread_attr_t attr;
    int disk_fd;
    struct MemoryBuffer** memory_buffers;
    int mem_buffers_count;
};

struct WriteParametersInThread2{
    pthread_attr_t attr;
    char* disk_buffer;
    struct MemoryBuffer* memory_buffer;
};
struct MultiWriteParametersInThread2{
    pthread_attr_t attr;
    char* disk_buffer;
    struct MemoryBuffer** memory_buffers;
    int mem_buffers_count;
};
/**
     * 일종의 wrapper함수로 내부적으로 위쪽의 
     * int write2Disk(int disk_fd,struct MemoryBuffer* memory_buffer); 를 호출함
     * 이때 내부에서 함수 파라매터 args를 
     * struct <Parmas>{
     *  pthread_attr_t attr;
     *  int disk_fd;
     *  struct MemoryBuffer* memory_buffer;
     * }
     * 로 변환해 write2Disk를 호출함
    */
int write2DiskInThread(void* args){
    struct WriteParametersInThread* params = (struct WriteParametersInThread*)args;
    int disk_fd = params->disk_fd;
    struct MemoryBuffer* mem_buffer = params->memory_buffer;
    int block_number = mem_buffer->header.block_number;
    free(params);
    int ret = write2Disk(disk_fd,mem_buffer);
    return ret;
}
int write2DiskMultipleInThread(void* args){
    struct MultiWriteParametersInThread* params = (struct MultiWriteParametersInThread*)args;
    int disk_fd = params->disk_fd;
    struct MemoryBuffer** mem_buffer = params->memory_buffers;
    int mem_buffers_count = params->mem_buffers_count;
    free(params);
    int ret = write2DiskMultiple(disk_fd,mem_buffer,mem_buffers_count);
    return ret;
}
int write2DiskInThread2(void* args){
    struct WriteParametersInThread2* params = (struct WriteParametersInThread2*)args;
    char* disk_buffer = params->disk_buffer;
    struct MemoryBuffer* mem_buffer = params->memory_buffer;
    int block_number = mem_buffer->header.block_number;
    free(params);
    int ret = write2Disk2(disk_buffer,mem_buffer);
    return ret;
}
int write2DiskMultipleInThread2(void* args){
    struct MultiWriteParametersInThread2* params = (struct MultiWriteParametersInThread2*)args;
    char* disk_buffer = params->disk_buffer;
    struct MemoryBuffer** mem_buffer = params->memory_buffers;
    int mem_buffers_count = params->mem_buffers_count;
    free(params);
    int ret = write2DiskMultiple2(disk_buffer,mem_buffer,mem_buffers_count);
    return ret;
}