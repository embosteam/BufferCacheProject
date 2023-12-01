 #ifndef __BUFFER_WRITE_HEADER__
    #define __BUFFER_WRITE_HEADER__
    #include "disk_write.h"
    #include "flush_thread.h"
    #include <stdio.h>
    #include <stdlib.h>
    /**
     * 버퍼를 파라메터에서 넘어온 값들을 기준으로 구현한 버퍼 저장 구조체에 넣어줌
     * 리턴값: buffer_content의 크기
    */
    int writeToBuffer(int disk_fd,struct MemoryBuffer* memory_buffer);

 #endif