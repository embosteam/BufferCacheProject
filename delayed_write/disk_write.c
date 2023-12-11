#include "disk_write.h"
/**
     * thread를 내부적으로 쓰레드를 사용하지 않으며
     * disk_fd위치를 dup()함수로 복사해서 memory_buffer의 실제 내용(char*)을 디스크에 쓰는 함수
    */
int write2Disk(int disk_fd,struct MemoryBuffer* memory_buffer){
    int ret;
    //printf("[write2Disk] (disk_fd:%d , blk_nr:%d , blocksize: %x)\n",disk_fd,memory_buffer->header.block_number,memory_buffer->header.block_size_byte);
    //printf("\t membuffer: %x\n",memory_buffer);
    struct MemoryBufferHeader* memory_buffer_header = &memory_buffer->header;
    if(memory_buffer_header->isBufferBeingWrittenNow){
        return (ret=0);
    }
    const int block_number = memory_buffer_header->block_number;
    const char* buffer_content = memory_buffer->buffer;
    const int block_size = memory_buffer_header->block_size_byte;
    int disk_fd2 = dup(disk_fd);
    if(disk_fd2==-1){
        perror("\t  error: ");
        return -1;
    }
    sem_wait(&memory_buffer->header.write_lock);
    if((&memory_buffer->header)->isDirty==0){
        sem_post(&memory_buffer->header.write_lock);
        return 0;
    }
    memory_buffer->header.isBufferBeingWrittenNow = 1;
    long long offset = block_number*block_size;
    ret = lseek(disk_fd2,offset,SEEK_SET);
    if(ret<0){
        sem_post(&memory_buffer->header.write_lock);
        perror("[write2Disk]file seek error");
        close(disk_fd2);
        return ret;
    }
    ret = write(disk_fd2,buffer_content,block_size);
    if(ret<0){
        sem_post(&memory_buffer->header.write_lock);
        close(disk_fd2);
        perror("[write2Disk]file write error");
        return ret;
    }
    close(disk_fd2);
    memory_buffer->header.isDirty = 0;
    sem_post(&memory_buffer->header.write_lock);
    memory_buffer->header.isBufferBeingWrittenNow = 0;
    
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
int write2DiskInThread(void* args){
    struct WriteParametersInThread* params = (struct WriteParametersInThread*)args;
    int disk_fd = params->disk_fd;
    struct MemoryBuffer* mem_buffer = params->memory_buffer;
    int block_number = mem_buffer->header.block_number;
    free(params);
    int ret = write2Disk(disk_fd,mem_buffer);
    //free(params);
    return ret;
}