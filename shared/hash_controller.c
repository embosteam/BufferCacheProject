#include "hash_controller.h"
uint8_t* MD5FetchHashValue(char* buffer,int length){
        MD5Context ctx;
        md5Init(&ctx);
        md5Update(&ctx,buffer,length);
        md5Finalize(&ctx);
        return ctx.digest;
    }
    unsigned char MD5IsEqualMd5Hash(uint8_t* c1,uint8_t* c2){
        if(c1==NULL || c2==NULL){
            return 0;
        }
        for(int i=0;i<16;i++){
            if(c1[i]!=c2[i]){
                return 0;
            }
        }
        return 1;
    }
    void MD5CopyHashValue(uint8_t* dest,uint8_t* src){
        memcpy(dest,src,16);
    }
struct MD5Controller* newMD5Controller(){
        struct MD5Controller* md5cont = (struct MD5Controller*)malloc(sizeof(struct MD5Controller));
        md5cont->fetchHashValue = MD5FetchHashValue;
        md5cont->isEqualMd5Hash = MD5IsEqualMd5Hash;
        md5cont->copyHashValue = MD5CopyHashValue;
        return md5cont;
    }
    