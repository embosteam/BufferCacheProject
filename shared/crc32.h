#pragma once
#ifndef __HEADER_CRC32__
    #define __HEADER_CRC32__
    #define CRC32_FAST
    unsigned int crc32(const unsigned char *message, unsigned int len);
#endif