#pragma once
#ifndef __FLUSH_THREAD__
#define __FLUSH_THREAD__
    #include <pthread.h>
    #include <semaphore.h>
    #include <unistd.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <time.h>
    #include <sys/time.h>
    #include "../shared/memory_buffer.h"

    struct FlushThreadGlobalVariable{
        pthread_t main_flush_thread_id;//thread를 통해 주기적으로 체크해주는 thread를 추가해줄 경우 사용
        pthread_cond_t main_flush_thread_condition;
        pthread_mutex_t main_flush_thread_main_mutex_locker;
    };
    /**
     * flush thread영역의 광역(>전역?!)변수들을 설정 및 정의
     * 
    */
    extern struct FlushThreadGlobalVariable global_variable_flush_thread;

    /**
     * runfunction(*arg) 에 해당하는 함수를 thread로 실행하게 해주는 함수
     * 리턴값: runfunction 함수를 실행하는 쓰레드의 id를 반환
    */
    pthread_t createAndRunFlushThread(int (*runfunction)(void*) ,void* arg);
#endif