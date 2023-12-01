#include "flush_thread.h"

/**
 * 매개변수 임시 할당을 위한 구조체
*/
struct WriteParametersInThread{
    pthread_attr_t attr;
    int disk_fd;
    struct MemoryBuffer* memory_buffer;
};
/**
     * runfunction(*arg) 에 해당하는 함수를 thread로 실행하게 해주는 함수
     * 리턴값: runfunction 함수를 실행하는 쓰레드의 id를 반환
    */
pthread_t createAndRunFlushThread(void* runfunction,void* arg){
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
    
    struct WriteParametersInThread params = *(struct WriteParametersInThread*)arg;
    params.attr = attr;
    if(pthread_create(&tid,NULL/*&attr*/,runfunction,&params)<0){
        perror("[createAndRunFlushThread] can not create flush thread\n");
        return 0;
    }
    return tid;
}
