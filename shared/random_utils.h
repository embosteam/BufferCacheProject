#pragma once
#ifndef __HEADER_RANDOM_UTILS__
    #define __HEADER_RANDOM_UTILS__
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <stdint.h>
    #include <time.h>
    #include <math.h>
    #include <limits.h>
    #include <memory.h>
    #define RANDOM_DISTRIBUTION_TYPE_PURE 0
    #define RANDOM_DISTRIBUTION_TYPE_NORMAL 1
    #define RANDOM_DISTRIBUTION_TYPE_ZIPFIAN 2
    
    

    /**
    * codes from : https://github.com/rflynn/c/blob/master/rand-normal-distribution.c
    * following gaussian distribution
    */
    double rand_normal();
    double rand_pure();
    double rand_zipfian();
    
    int getRandomValue(unsigned char random_distribution_type);


#endif