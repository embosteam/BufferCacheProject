#include "flush_thread.h"

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
    int mem_buffer_count;
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
    int mem_buffer_count;
};
/**
     * runfunction(*arg) 에 해당하는 함수를 thread로 실행하게 해주는 함수
     * 리턴값: runfunction 함수를 실행하는 쓰레드의 id를 반환
    */
pthread_t createAndRunFlushThread(int (*runfunction)(void*),void* arg){
    /**
     * thread 종료시 thread가 알아서 자원을 반환하도록 하는 attribute
    */
   pthread_attr_t attr;
   pthread_t tid;
   /*
   //thread 종료시,해당 thread 자원 자동 반환 구현중
   if(pthread_attr_init(&attr)!=0){
        return 0;
   }
   if(pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED)!=0){
    return 0;
   }
   */
    struct WriteParametersInThread* params = (struct WriteParametersInThread*)arg;
    //params.attr = attr;
    if(pthread_create(&tid,NULL/*&attr*/,runfunction,params)<0){
        perror("[createAndRunFlushThread] can not create flush thread\n");
        return 0;
    }
    return tid;
}

pthread_t flushBufferWithThread(int disk_fd,struct MemoryBuffer* mem_buffer){
    pthread_t tid;
    struct WriteParametersInThread* params = (struct WriteParametersInThread*)malloc(sizeof(struct WriteParametersInThread));
    params->disk_fd = disk_fd;
    params->memory_buffer = mem_buffer;
   // printf("  [flushBufferWithThread] start %d(%x)\n",mem_buffer->header.block_number,mem_buffer->buffer);
    
    if(pthread_create(&tid,NULL/*&attr*/,write2DiskInThread,params)<0){
        perror("\t[flushBufferWithThread] can not create flush thread\n");
        printf("\t\t[flushBufferWithThread] create error- blk: %d\n",mem_buffer->header.block_number);
        return 0;
    }
    //printf("  [flushBufferWithThread] started %d(%x)\n",mem_buffer->header.block_number,mem_buffer->buffer);
    
    return tid;
}
pthread_t flushBuffersWithThread(int disk_fd,struct MemoryBuffer** mem_buffers,int mem_buffer_count){
    pthread_t tid;
    struct MultiWriteParametersInThread* params = (struct MultiWriteParametersInThread*)malloc(sizeof(struct MultiWriteParametersInThread));
    params->disk_fd = disk_fd;
    params->memory_buffers = mem_buffers;
    params->mem_buffer_count = mem_buffer_count;
    //printf("  [flushBuffersWithThread] start %d (%x)\n",mem_buffers[0]->header.block_number,mem_buffers[0]->buffer);
    if(pthread_create(&tid,NULL/*&attr*/,write2DiskMultipleInThread,params)<0){
        perror("[flushBuffersWithThread] can not create flush thread\n");
        return 0;
    }
    //printf("  [flushBuffersWithThread] started %d (%x)\n",mem_buffers[0]->header.block_number,mem_buffers[0]->buffer);
    return tid;
}

pthread_t flushBufferWithThread2(char* disk_buffer,struct MemoryBuffer* mem_buffer){
    pthread_t tid;
    struct WriteParametersInThread2* params = (struct WriteParametersInThread2*)malloc(sizeof(struct WriteParametersInThread2));
    params->disk_buffer = disk_buffer;
    params->memory_buffer = mem_buffer;

    if(pthread_create(&tid,NULL/*&attr*/,write2DiskInThread2,params)<0){
        perror("[flushBufferWithThread] can not create flush thread\n");
        return 0;
    }
    //pthread_t* tid_wrapper = malloc(sizeof(pthread_t))
    return tid;
}
pthread_t flushBuffersWithThread2(char* disk_buffer,struct MemoryBuffer** mem_buffers,int mem_buffer_count){
    pthread_t tid;
    struct MultiWriteParametersInThread2* params = (struct MultiWriteParametersInThread2*)malloc(sizeof(struct MultiWriteParametersInThread2));
    params->disk_buffer = disk_buffer;
    params->memory_buffers = mem_buffers;
    params->mem_buffer_count = mem_buffer_count;
    if(pthread_create(&tid,NULL/*&attr*/,write2DiskMultipleInThread2,params)<0){
        perror("[flushBuffersWithThread] can not create flush thread\n");
        return 0;
    }
    return tid;
}