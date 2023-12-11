#include "thread_pool.h"


/**
 * 매개변수 임시 할당을 위한 구조체
*/
struct WriteParametersInThread{
    pthread_attr_t attr;
    int disk_fd;
    struct MemoryBuffer* memory_buffer;
};
struct ThreadParam{
    struct ThreadPool* tp;
};
pthread_t startMainPooling(struct ThreadPool* tp){
    pthread_t tid = 0;
    struct ThreadParam* param = (struct ThreadParam*)malloc(sizeof(struct ThreadParam));
    param->tp = tp;
    if(pthread_create(&tid,NULL/*&attr*/,
    #ifdef __APPLE__
        &ThreadPoolMainPooling1
    #else
        tp->pooling?tp->pooling:&ThreadPoolMainPooling1
    #endif
    
    ,(void*)param)<0){
        perror("[startMainPooling] can not create flush thread\n");
        return 0;
    }
    return tid;
}
/**
     * ThreadPool 인스턴스를 생성
    */

    struct ThreadPool* newThreadPool(int n){

        struct ThreadPool* tp = (struct ThreadPool*)malloc(sizeof(struct ThreadPool));
        
        tp->number_of_thread = n;
        
        sem_init(&tp->thread_unit_queue_lock,0,n);
        
        //tp->thread_unit_queue_lock = sem_open("unit_queue_lock",O_CREAT|O_EXCL,0600,n);
        sem_init(&tp->thread_running_queue_lock,0,1);
        
        sem_init(&tp->thread_should_terminate_state_lock,0,1);
        
        tp->thread_should_terminate_state = 0;

        tp->running_queue = newLQueue(1);
        
        tp->thread_unit_queue = newLQueue(1);
        tp->pooling = ThreadPoolMainPooling1;
        tp->releaseInternalResource = ThreadPoolReleaseInternalResource;
        tp->addQueue = ThreadPoolAddQueue;
        tp->shouldTerminateThreadPool = ThreadPoolShouldTerminateThreadPool;
        tp->waitInternalThread = ThreadPoolWaitInternalThread;
        tp->close = ThreadPoolClose;
        
        
        tp->pooling_tid = 0;

        tp->pooling_tid = startMainPooling(tp);
        printf("[tp pooling] created\n");
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
    /**
     * threadpool 폴링을 하는 함수
    */
    void ThreadPoolMainPooling1(void* arg){
         struct ThreadParam* function_param = (struct ThreadParam*)arg;
        struct ThreadPool* tp = (struct ThreadPool*)(function_param->tp);
        free(function_param);
        while(!tp->shouldTerminateThreadPool(tp)){
            
            
            if(!(tp->thread_unit_queue->isEmpty(tp->thread_unit_queue))){
                int try_lock = try_semalock(&tp->thread_unit_queue_lock);
                
                if(!try_lock){
                
                    struct QueueUnit1* item = (struct QueueUnit1*)(tp->thread_unit_queue->front(tp->thread_unit_queue));
                    (tp->thread_unit_queue->pop(tp->thread_unit_queue));
                    struct WriteParametersInThread* params =(struct WriteParametersInThread*)malloc(sizeof(struct WriteParametersInThread));
                    params->disk_fd = item->disk_fd;
                    params->memory_buffer = item->buffer;
                    

                    item->buffer = NULL;
                    pthread_t tid = createAndRunFlushThread(write2DiskInThread,params);
                    free(item);
                    pthread_t* tid_wrapper = (pthread_t*)malloc(sizeof(pthread_t));
                    tid_wrapper[0] = tid;
                    tp->running_queue->push(tp->running_queue,tid_wrapper);
                }else{
                   tp->waitInternalThread(tp,1);
                }
            }
            else{
                usleep(1);
            }
        }
        printf("[ThreadPoolMainPooling1] finished\n");
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
    void ThreadPoolAddQueue(struct ThreadPool* tp,int disk_fd,struct MemoryBuffer* buffer){
        //sem_wait(&tp->thread_running_queue_lock);fd);
        struct QueueUnit1* item  = (struct QueueUnit1*)malloc(sizeof(struct QueueUnit1));
        item->disk_fd = disk_fd;
        item->buffer = buffer;
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
        if(tp->pooling_tid!=0){
            //usleep(1000*100);id);
            pthread_join(tp->pooling_tid,&tid_status);//(void**)&tid_status);
        }
        tp->waitInternalThread(tp,0);
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