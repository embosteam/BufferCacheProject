#include "thread_pool.h"


/**
 * 매개변수 임시 할당을 위한 구조체
*/
struct WriteParametersInThread{
    pthread_attr_t attr;
    int disk_fd;
    struct MemoryBuffer* memory_buffer;
};
struct WriteParametersInThread2{
    pthread_attr_t attr;
    char* disk_buffer;
    struct MemoryBuffer* memory_buffer;
};
struct MultiWriteParametersInThread{
    pthread_attr_t attr;
    int disk_fd;
    struct MemoryBuffer** memory_buffers;
};
struct ThreadParam{
    struct ThreadPool* tp;
};
pthread_t startMainPooling(struct ThreadPool* tp){
    pthread_t tid = 0;
    struct ThreadParam* param = (struct ThreadParam*)malloc(sizeof(struct ThreadParam));
    param->tp = tp;
    printf("[startMainPooling] starting..\n");
    if(pthread_create(&tid,NULL/*&attr*/,tp->pooling?tp->pooling:&ThreadPoolMainPooling1,(void*)param)<0){
        perror("[startMainPooling] can not create flush thread\n");
        return 0;
    }
    return tid;
}
#if(USE_MMAP_RW!=0)
    typedef struct QueueUnit2 CurrentQueueUnit ;
#else
    typedef struct QueueUnit1 CurrentQueueUnit ;
#endif
int fetchKeyFromValue(struct LHeap* internal_heap_self,void* obj){
    CurrentQueueUnit* item = (CurrentQueueUnit*)obj;
    
    return item->block_number;
}
int lhq_comparator(struct LHeap* self,void* s1,void* s2){
        if(s1!=NULL&&s2!=NULL){
            struct LHeapInternalNode* node1 = s1;//(( struct LHeapInternalNode*)s1)
            struct LHeapInternalNode* node2 = s2;
            CurrentQueueUnit* qu1 = node1->value;
            CurrentQueueUnit* qu2 = node2->value;
            if(qu1->priority==qu2->priority){
                const int v1 = self->fetchKeyFromValue(self,node1->value);
                const int v2 = self->fetchKeyFromValue(self,node2->value);
                return v1-v2;
            }
            else{
                return qu2->priority-qu1->priority;
            }
        }
        else if(s1!=NULL){
            return 1;
        }
        else if(s2!=NULL){
            return -1;
        }
        else{
            return 0;
        }
    }
int comparator4AQueue(void* s1,void* s2){
    if(s1!=NULL&&s2!=NULL){
        struct QueueUnit1* item1 = (struct QueueUnit1*)s1;
        struct QueueUnit1* item2 = (struct QueueUnit1*)s2;
        int blk_number1 = item1->block_number;
        int blk_number2 = item2->block_number;
         if(blk_number1<blk_number2){
            return 1;
        }
        else if(blk_number1>blk_number2){
            return -1;
        }
        else{
            return 0;
        }
    }
    else if(s1!=NULL){
         
        return -1;
    }
    else if(s2!=NULL){
        return 1;
    }
    else{
        return 0;
    }

}
/**
     * ThreadPool 인스턴스를 생성
    */

    struct ThreadPool* newThreadPool(int n,unsigned int max_buffer_continous_sequential_length){

        struct ThreadPool* tp = (struct ThreadPool*)malloc(sizeof(struct ThreadPool));
        
        tp->number_of_thread = n;
        tp->max_buffer_continous_sequential_length = max_buffer_continous_sequential_length;
        
        sem_init(&tp->thread_unit_queue_lock,0,n);
        
        //tp->thread_unit_queue_lock = sem_open("unit_queue_lock",O_CREAT|O_EXCL,0600,n);
        sem_init(&tp->thread_running_queue_lock,0,1);
        
        sem_init(&tp->thread_should_terminate_state_lock,0,1);
        
        tp->thread_should_terminate_state = 0;

        tp->running_queue = newLQueue(1);
        
        //tp->thread_unit_queue = newLQueue(1);
        //tp->thread_unit_queue = newAQueue(2048,1,comparator4AQueue);
        tp->thread_unit_queue = newLHQueue(1,fetchKeyFromValue,lhq_comparator); //newLHQueue2(1,fetchKeyFromValue);
        //tp->pooling = ThreadPoolMainPooling1;
        tp->releaseInternalResource = ThreadPoolReleaseInternalResource;
        tp->addQueue = ThreadPoolAddQueue;
        tp->addQueue2 = ThreadPoolAddQueue2;
        tp->shouldTerminateThreadPool = ThreadPoolShouldTerminateThreadPool;
        tp->waitInternalThread = ThreadPoolWaitInternalThread;
        tp->close = ThreadPoolClose;
        
        
        tp->pooling_tid = 0;

        tp->pooling_tid = startMainPooling(tp);
        return tp;
    }
    /**
     * 클래스 destructor 함수에 해당하는 함수
    */
    void ThreadPoolReleaseInternalResource(struct ThreadPool* tp){
        
        tp->close(tp);
        
        tp->running_queue->free(tp->running_queue);
        
        free(tp->running_queue);
        tp->thread_unit_queue->free(tp->thread_unit_queue);
        free(tp->thread_unit_queue);
        //sem_destroy(tp->thread_unit_queue_lock);
        
        sem_destroy(&tp->thread_unit_queue_lock);
        sem_destroy(&tp->thread_running_queue_lock);
        sem_destroy(&tp->thread_should_terminate_state_lock);
    }
    char* getErrorCodeStr(int error_number){
        switch(error_number){
            case EINVAL:{
                return "EINVAL\0";
            }
            case EAGAIN:{
                return "EAGAIN\0";
            }
            case EDEADLK:{
                return "EDEADLK\0";
            }
            case EINTR:{
                return "EINTR\0";
            }
            case EACCES:{
                return "EACCES\0";
            }
            case EFAULT:{
                return "EFAULT\0";
            }
            case EIDRM:{
                return "EIDRM\0";
            }
            case ENOMEM:{
                return "ENOMEM\0";
            }
            case ENOTSUP:{
                return "ENOTSUP\0";
            }
            case EMFILE:{
                return "EMFILE\0";
            }
            case ENFILE:{
                return "ENOTSUP\0";
            }
            case ENOENT:{
                return "ENOENT\0";
            }

        }
        return "unknown\0";
    }
    int try_semalock(sem_t* sem){
        int sema_value;
        sema_value = sem_trywait(sem);
        while((sema_value==-1)&&(errno==EINTR)){
            sema_value = sem_trywait(sem);
        }
        return sema_value;
    }
    struct MemoryBuffer** newContinuousMemoryBuffers(struct ThreadPool* tp){
        struct MemoryBuffer** continuous_memory_buffers = (struct MemoryBuffer**)malloc(sizeof(struct MemoryBuffer*)*(tp->max_buffer_continous_sequential_length+1));
        memset(continuous_memory_buffers,0,sizeof(struct MemoryBuffer*)*(tp->max_buffer_continous_sequential_length+1));
        return continuous_memory_buffers;
    }
    pthread_t flushContinuousBuffers(struct ThreadPool* tp,int disk_fd,struct MemoryBuffer** memory_buffers,int mem_buffers_count){
        printf("[flushContinuousBuffers] start %d ~ %d\n",memory_buffers[0]->header.block_number,memory_buffers[0]->header.block_number+mem_buffers_count-1);
        return flushBuffersWithThread(disk_fd,memory_buffers, mem_buffers_count);
    }
    pthread_t flushContinuousBuffers2(struct ThreadPool* tp,char* disk_buffer,struct MemoryBuffer** memory_buffers,int mem_buffers_count){

        return flushBuffersWithThread2(disk_buffer,memory_buffers, mem_buffers_count);
    }
    /**
     * threadpool 폴링을 하는 함수
    */
    void ThreadPoolMainPooling1(void* arg){
         struct ThreadParam* function_param = (struct ThreadParam*)arg;
        struct ThreadPool* tp = (struct ThreadPool*)(function_param->tp);
        struct MemoryBuffer** continuous_memory_buffers =newContinuousMemoryBuffers(tp);
        unsigned int continuous_memory_buffers_count = 0;
        free(function_param);
        int disk_fd;
        char* disk_buffer = NULL;
        int processed_count = 0;
        struct MemoryBuffer* tmp_buffer = malloc(sizeof(struct MemoryBuffer));
        tmp_buffer->header.isValid=0;
        tmp_buffer->header.isDirty=0;
        tmp_buffer->header.block_number=-1;
        for(int k=0;k<30;k++){
            pthread_t tid = flushBufferWithThread(-1,tmp_buffer);
            usleep(1000);
        }
        int ordered_count=0;
        while(!tp->shouldTerminateThreadPool(tp)){
            if(!(tp->thread_unit_queue->isEmpty(tp->thread_unit_queue))){
                int try_lock = try_semalock(&tp->thread_unit_queue_lock);
                if(!try_lock){
                    void* front_value = tp->thread_unit_queue->front(tp->thread_unit_queue);
                     (tp->thread_unit_queue->pop(tp->thread_unit_queue));
                    //printf("[ThreadPoolMainPooling1] isNull: %d\n",front_value==NULL);
                    if(front_value==NULL){
                        sem_post(&tp->thread_unit_queue_lock);
                        continue;
                    }
                    //struct QueueUnit1* item = (struct QueueUnit1*)(front_value);
                    CurrentQueueUnit* item = (CurrentQueueUnit*)(front_value);
                    
                    
                    processed_count++;
                    //printf("[queue debug] mem_buffer:%x buffer: %x , isBuffering: %d, isValid: %d, isDirty: %d\n",
                    //item->buffer,item->buffer->buffer,item->buffer->header.isBufferBeingWrittenNow,item->buffer->header.isValid,item->buffer->header.isDirty);
                    if(item->buffer!=NULL&&item->buffer->header.isValid==0){
                        sem_post(&tp->thread_unit_queue_lock);
                        continue;
                    }
                    if(item->buffer==NULL||(item->buffer!=NULL&&(item->buffer->header.isBufferBeingWrittenNow == 1||item->buffer->header.isDirty == 0||item->buffer->header.isValid==0||item->buffer->buffer==NULL))){
                        /*if(item->buffer!=NULL){
                        printf("  [ThreadPoolMainPooling1] skipped %d (%d) (isDirty: %d) (isValid: %d) (isBufferNULL: %d) (isBufferBeingWrittenNow: %d)\n",item->buffer->header.block_number,item->buffer->header.request_write_count,item->buffer->header.isDirty,item->buffer->header.isValid,item->buffer->buffer==NULL,item->buffer->header.isBufferBeingWrittenNow);
                            printf("\tbuffering list: (cnt:%d)\n",continuous_memory_buffers_count);
                            for(int k=0;k<continuous_memory_buffers_count;k++){
                            printf("\t\tblk: %d\n",continuous_memory_buffers[k]->header.block_number);
                            }
                        }*/
                        sem_post(&tp->thread_unit_queue_lock);
                        continue;
                    }
                    sem_wait(&item->buffer->header.write_lock);
                    
                    //
                    ordered_count++;
                    if(CONFIG_DEBUG_MODE){
                    printf("[ThreadPoolMainPooling1] picked %d (%d)\n",item->buffer->header.block_number,item->buffer!=NULL);
                    }
                   // printf("[ThreadPoolMainPooling1] picked %d (%d)\n",item->buffer->header.block_number,item->buffer!=NULL);
                    
                    /*struct WriteParametersInThread2* params =(struct WriteParametersInThread2*)malloc(sizeof(struct WriteParametersInThread2));
                    //params->disk_fd = item->disk_fd;
                    disk_fd = item->disk_fd;*/
                    //disk_fd = item->disk_fd;
                    //params->disk_buffer = item->disk_buffer;
                   // printf("disk buffer start\n");
                   #if(USE_MMAP_RW==0)
                    disk_fd = item->disk_fd;
                    #else
                    disk_buffer = item->disk_buffer;
                    #endif
                    //printf("disk buffer end\n");
                    //params->memory_buffer = item->buffer;
                    //printf("\tpass1\n");
                    if(tp->max_buffer_continous_sequential_length>0){

                        //printf("\tunit queue size: %d , unit queue pointer index: %d\n",tp->thread_unit_queue->getSize(tp->thread_unit_queue));//,tp->thread_unit_queue->current_pointer_index);
                        //item->buffer = NULL;
                        //pthread_t tid = createAndRunFlushThread(write2DiskInThread,params);
                        const int current_item_block_number = item->block_number;
                        if(continuous_memory_buffers_count==0){
                            continuous_memory_buffers[continuous_memory_buffers_count++] = item->buffer;
                            item->buffer->header.isBufferBeingWrittenNow = 1;
                            item->buffer->header.request_write_count++;
                            sem_post(&item->buffer->header.write_lock);
                            sem_post(&tp->thread_unit_queue_lock);
                            
                        }
                        else if(continuous_memory_buffers_count+1>=tp->max_buffer_continous_sequential_length){
                            int continous_last_blk_number= continuous_memory_buffers[continuous_memory_buffers_count-1]->header.block_number;
                            if(continous_last_blk_number+1==current_item_block_number){
                                continuous_memory_buffers[continuous_memory_buffers_count++] = item->buffer;
                                item->buffer->header.isBufferBeingWrittenNow = 1;
                                item->buffer->header.request_write_count++;
                                sem_post(&item->buffer->header.write_lock);
                                #if(USE_MMAP_RW!=0)
                                pthread_t tid = flushContinuousBuffers2(tp,disk_buffer,continuous_memory_buffers,continuous_memory_buffers_count);
                                #else
                                pthread_t tid = flushContinuousBuffers(tp,disk_fd,continuous_memory_buffers,continuous_memory_buffers_count);
                                #endif                                
                                pthread_t* tid_wrapper = (pthread_t*)malloc(sizeof(pthread_t));
                                tid_wrapper[0] = tid;
                                tp->running_queue->push(tp->running_queue,tid_wrapper);
                                usleep(10*1000);
                                continuous_memory_buffers_count = 0;
                                continuous_memory_buffers = newContinuousMemoryBuffers(tp);
                                //sem_post(&tp->thread_unit_queue_lock);
                                //continuous_memory_buffers[continuous_memory_buffers_count] = NULL;
                                //TODO:: run flush multiple
                            }
                            else if(continous_last_blk_number==current_item_block_number){
                                sem_post(&item->buffer->header.write_lock);
                                sem_post(&tp->thread_unit_queue_lock);
                            }
                            else{
                                //continuous_memory_buffers[continuous_memory_buffers_count] = NULL;
                                //TODO:: run flush multiple
                                #if(USE_MMAP_RW!=0)
                                pthread_t tid = flushContinuousBuffers2(tp,disk_buffer,continuous_memory_buffers,continuous_memory_buffers_count);
                                #else
                                pthread_t tid = flushContinuousBuffers(tp,disk_fd,continuous_memory_buffers,continuous_memory_buffers_count);
                                #endif                                
                                pthread_t* tid_wrapper = (pthread_t*)malloc(sizeof(pthread_t));
                                tid_wrapper[0] = tid;
                                tp->running_queue->push(tp->running_queue,tid_wrapper);
                                continuous_memory_buffers_count = 0;
                                continuous_memory_buffers = newContinuousMemoryBuffers(tp);
                                usleep(10*1000);
                                item->buffer->header.isBufferBeingWrittenNow = 1;
                                item->buffer->header.request_write_count++;
                                sem_post(&item->buffer->header.write_lock);
                                continuous_memory_buffers[continuous_memory_buffers_count++] = item->buffer;
                            }
                            
                        }
                        else{
                            int continous_last_blk_number= continuous_memory_buffers[continuous_memory_buffers_count-1]->header.block_number;
                            //printf("\t[ThreadPoolMainPooling1] last_blk: %d , current_blk: %d\n",continous_last_blk_number,current_item_block_number);
                            if(continous_last_blk_number+1==current_item_block_number){
                                item->buffer->header.isBufferBeingWrittenNow = 1;
                                item->buffer->header.request_write_count++;
                                sem_post(&item->buffer->header.write_lock);
                                continuous_memory_buffers[continuous_memory_buffers_count++] = item->buffer;
                                sem_post(&tp->thread_unit_queue_lock);
                            }
                            else if(continous_last_blk_number==current_item_block_number){
                                sem_post(&item->buffer->header.write_lock);
                                sem_post(&tp->thread_unit_queue_lock);
                            }
                            else{
                                //continuous_memory_buffers[continuous_memory_buffers_count] = NULL;
                                //TODO:: run flush multiple
                                //printf("\t\t[ThreadPoolMainPooling1] run flush(count=%d)\n",continuous_memory_buffers_count);
                                #if(USE_MMAP_RW!=0)
                                pthread_t tid = flushContinuousBuffers2(tp,disk_buffer,continuous_memory_buffers,continuous_memory_buffers_count);
                                #else
                                pthread_t tid = flushContinuousBuffers(tp,disk_fd,continuous_memory_buffers,continuous_memory_buffers_count);
                                #endif
                                pthread_t* tid_wrapper = (pthread_t*)malloc(sizeof(pthread_t));
                                tid_wrapper[0] = tid;
                                tp->running_queue->push(tp->running_queue,tid_wrapper);
                                usleep(10*1000);
                                continuous_memory_buffers_count = 0;
                                continuous_memory_buffers = newContinuousMemoryBuffers(tp);
                                item->buffer->header.isBufferBeingWrittenNow = 1;
                                item->buffer->header.request_write_count++;
                                sem_post(&item->buffer->header.write_lock);
                                continuous_memory_buffers[continuous_memory_buffers_count++] = item->buffer;
                            }
                            
                        }
                    }
                    else{
                        /*struct WriteParametersInThread2* params =(struct WriteParametersInThread2*)malloc(sizeof(struct WriteParametersInThread2));
                        params->disk_buffer = item->disk_buffer;
                        //disk_fd = item->disk_fd;
                         params->memory_buffer = item->buffer;*/

                       /*  struct WriteParametersInThread* params =(struct WriteParametersInThread*)malloc(sizeof(struct WriteParametersInThread));
                        params->disk_fd = item->disk_fd;
                        //disk_fd = item->disk_fd;
                         params->memory_buffer = item->buffer;*/
                        (&item->buffer->header)->isBufferBeingWrittenNow = 1;
                        item->buffer->header.request_write_count++;
                        sem_post(&item->buffer->header.write_lock);
                        #if(USE_MMAP_RW!=0)
                        pthread_t tid = flushBufferWithThread2(disk_buffer,item->buffer); //createAndRunFlushThread2(write2DiskInThread,params);
                        #else
                        pthread_t tid = flushBufferWithThread(disk_fd,item->buffer); //createAndRunFlushThread2(write2DiskInThread,params);
                        #endif                        
                        //pthread_t tid = createAndRunFlushThread(write2DiskInThread,params); //flushContinuousBuffers(tp,disk_fd,continuous_memory_buffers,continuous_memory_buffers_count);
                    
                        pthread_t* tid_wrapper = (pthread_t*)malloc(sizeof(pthread_t));
                        tid_wrapper[0] = tid;
                        tp->running_queue->push(tp->running_queue,tid_wrapper);
                        usleep(10*1000);
                      
                    }
                    item->buffer = NULL;
                    free(item);
                    //pthread_t* tid_wrapper = (pthread_t*)malloc(sizeof(pthread_t));
                    //tid_wrapper[0] = tid;
                    //tp->running_queue->push(tp->running_queue,tid_wrapper);
                }else{
                   tp->waitInternalThread(tp,1);
                }
            }
            else{
                
                //printf("[main polling] isEmpty: %d count: %d\n",tp->thread_unit_queue->isEmpty(tp->thread_unit_queue),continuous_memory_buffers_count);
                if(continuous_memory_buffers_count>0){
                    #if(USE_MMAP_RW!=0)
                    pthread_t tid = flushContinuousBuffers2(tp,disk_buffer,continuous_memory_buffers,continuous_memory_buffers_count);
                    #else
                    pthread_t tid = flushContinuousBuffers(tp,disk_fd,continuous_memory_buffers,continuous_memory_buffers_count);
                    #endif                    
                    pthread_t* tid_wrapper = (pthread_t*)malloc(sizeof(pthread_t));
                    tid_wrapper[0] = tid;
                    tp->running_queue->push(tp->running_queue,tid_wrapper);
                    continuous_memory_buffers_count = 0;
                    continuous_memory_buffers = newContinuousMemoryBuffers(tp);
                }
                //usleep(2*500*1000);
                usleep(750*1000);
                //usleep(1);
               // tp->thread_unit_queue->sort(tp->thread_unit_queue);
            }
        }
        if(continuous_memory_buffers_count>0){
            #if(USE_MMAP_RW!=0)
            pthread_t tid = flushContinuousBuffers2(tp,disk_buffer,continuous_memory_buffers,continuous_memory_buffers_count);
            #else
            pthread_t tid = flushContinuousBuffers(tp,disk_fd,continuous_memory_buffers,continuous_memory_buffers_count);
            #endif            
            pthread_t* tid_wrapper = (pthread_t*)malloc(sizeof(pthread_t));
            tid_wrapper[0] = tid;
            tp->running_queue->push(tp->running_queue,tid_wrapper);
        }
        //tp->thread_unit_queue->sort(tp->thread_unit_queue);
        printf("[ThreadPoolMainPooling1] finished(%d::%d)\n",processed_count,ordered_count);
        //return NULL;
    }

    /**
     * 모든 쓰레드가 종료될때까지 기다리는 함수
    */
    void ThreadPoolWaitInternalThread(struct ThreadPool* tp,unsigned char pooling_mode){
        struct LQueue* rq = tp->running_queue;
        while(!rq->isEmpty(rq)){
            pthread_t* raw_wrapper = ((pthread_t*)rq->front(rq));
            const pthread_t  raw = raw_wrapper[0];
            rq->pop(rq);
            
            free(raw_wrapper);
            long long status;
            pthread_join(raw,&status);
            
            printf("[ThreadPoolWaitInternalThread] subthread %d finished\n",raw);
            if(pooling_mode>0){
                sem_post(&tp->thread_unit_queue_lock);
                break;
            }
        }
    }

    /**
     * threadpool에 작업 단위를 추가하는 내부 함수
    */
    void ThreadPoolAddQueue(struct ThreadPool* tp,int disk_fd,struct MemoryBuffer* buffer,int priority){
        //sem_wait(&tp->thread_running_queue_lock);fd);
        struct QueueUnit1* item  = (struct QueueUnit1*)malloc(sizeof(struct QueueUnit1));
        memset(item,0,sizeof(struct QueueUnit1));
        item->priority = priority;
        item->disk_fd = disk_fd;
        item->buffer = buffer;
        item->block_number=buffer->header.block_number;
        tp->thread_unit_queue->push(tp->thread_unit_queue,item);
        //sem_post(&tp->thread_running_queue_lock);
    }

     void ThreadPoolAddQueue2(struct ThreadPool* tp,char* disk_buffer,struct MemoryBuffer* buffer,int priority){
        //sem_wait(&tp->thread_running_queue_lock);fd);
        struct QueueUnit2* item  = (struct QueueUnit2*)malloc(sizeof(struct QueueUnit2));
        memset(item,0,sizeof(struct QueueUnit2));
        item->priority = priority;
        item->disk_buffer = disk_buffer;
        item->buffer = buffer;
        //printf("\t[ThreadPoolAddQueue2] buffer: %x\n",buffer);
        item->block_number=buffer->header.block_number;
        tp->thread_unit_queue->push(tp->thread_unit_queue,item);
        //sem_post(&tp->thread_running_queue_lock);
    }
    /**
     * thread.join을 통해 thread를 정리하는 함수
    */
    void ThreadPoolClose(struct ThreadPool* tp){
        long long tid_status;
        sem_wait(&tp->thread_should_terminate_state_lock);
        tp->thread_should_terminate_state = 1;
        
        sem_post(&tp->thread_should_terminate_state_lock);
        printf("[ThreadPoolClose] joining pooling threads...\n");
        if(tp->pooling_tid!=0){
            //usleep(1000*100);id);
            pthread_join(tp->pooling_tid,&tid_status);//(void**)&tid_status);
        }
        printf("[ThreadPoolClose] finished joining pooling threads...\n");
        printf("[ThreadPoolClose] joining sub threads...\n");
        tp->waitInternalThread(tp,0);
        printf("[ThreadPoolClose] finished sub threads...\n");
        tp->running_queue->clear(tp->running_queue);
        tp->thread_unit_queue->clear(tp->thread_unit_queue);
    }
    /**
     * pooling 을 그만두어야 할지 상태를 체크하는 함수
     * 리턴값: 참/거짓
    */
    int ThreadPoolShouldTerminateThreadPool(struct ThreadPool* tp){
        sem_wait(&tp->thread_should_terminate_state_lock);
        const int value = tp->thread_should_terminate_state;
        sem_post(&tp->thread_should_terminate_state_lock);
        return value;
    }