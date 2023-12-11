#pragma once
#ifndef __HEADER_HASH_CONTROLLER__
    #define __HEADER_HASH_CONTROLLER__
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <memory.h>
    #include "md5.h"
    
    struct MD5Controller{
        uint8_t* (*fetchHashValue)(char*,int);

        unsigned char (*isEqualMd5Hash)(uint8_t*,uint8_t*);
        void (*copyHashValue)(uint8_t*,uint8_t*);
    };
    struct MD5Controller* newMD5Controller();
#endif