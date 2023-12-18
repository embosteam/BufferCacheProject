#include "time_utils.h"
unsigned long long getCurrentTime(){
        struct timespec tspec;
        clock_gettime(CLOCK_REALTIME,&tspec);
        time_t tm = time(NULL);
        unsigned long long result = (unsigned long long)(tspec.tv_sec?tspec.tv_sec:tm)*1000*1000*1000+(unsigned long long)tspec.tv_nsec;
        return result;
    }