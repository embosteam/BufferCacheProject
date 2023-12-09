#pragma once
#ifndef __HEADER_THREAD_POOL__
    #define __HEADER_THREAD_POOL__
    #include <pthread.h>
    #include <semaphore.h>
    #include <unistd.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <memory.h>
    #include <time.h>
    #include <sys/time.h>
    #include "../shared/memory_buffer.h"
    #include "../shared/linked_queue.h"
    #include "disk_write.h"
    #include "flush_thread.h"
    struct QueueUnit1{
        int disk_fd;
        int block_number;
        struct MemoryBuffer* buffer;
    };
    struct ThreadPool{
        sem_t thread_running_queue_lock;
        sem_t thread_unit_queue_lock;
        sem_t thread_should_terminate_state_lock;
        int thread_should_terminate_state;
        struct LQueue* running_queue;
        struct LQueue* thread_unit_queue; //priority_queue로 바뀔 예정
        int number_of_thread;
        pthread_t pooling_tid;
        void (*pooling)(void*);
        void (*releaseInternalResource)(struct ThreadPool*);
        void (*close)(struct ThreadPool*);
        void (*waitInternalThread)(struct ThreadPool*,unsigned char);
        void (*addQueue)(struct ThreadPool*,int,struct MemoryBuffer*);
        int (*shouldTerminateThreadPool)(struct ThreadPool*);
    };

    /**
     * ThreadPool 인스턴스를 생성
    */
    struct ThreadPool* newThreadPool(int n);
    /**
     * 클래스 destructor 함수에 해당하는 함수
    */
    void ThreadPoolReleaseInternalResource(struct ThreadPool* tp);
    /**
     * threadpool 폴링을 하는 함수
    */
    void ThreadPoolMainPooling1(void* parm);

    /**
     * threadpool에 작업 단위를 추가하는 내부 함수
    */
    void ThreadPoolAddQueue(struct ThreadPool* tp,int disk_fd,struct MemoryBuffer* buffer);
   
   /**
     * 모든 쓰레드가 종료될때까지 기다리는 함수
    */
    void ThreadPoolWaitInternalThread(struct ThreadPool* tp,unsigned char);
   
   /**
     * thread.join을 통해 thread를 정리하는 함수
    */
    void ThreadPoolClose(struct ThreadPool* tp);
    /**
     * pooling 을 그만두어야 할지 상태를 체크하는 함수
     * 리턴값: 참/거짓
    */
    int ThreadPoolShouldTerminateThreadPool(struct ThreadPool* tp);

#endif