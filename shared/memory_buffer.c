#include "memory_buffer.h"
struct MemoryBuffer* createNewMemoryBuffer(int block_size_byte,int block_number){
        struct MemoryBuffer* buffer = NULL;
        buffer = (struct MemoryBuffer*)malloc(sizeof(struct MemoryBuffer));
        memset(buffer,0,sizeof(struct MemoryBuffer));
        buffer->buffer = (char*)malloc(block_size_byte);//aligned_alloc(block_size,block_size*sizeof(char));
        (buffer->header.block_number) = block_number;
        buffer->header.block_size_byte = block_size_byte;
        
        //printf("[createNewMemoryBuffer] isBufferNULL: %d\n",buffer->buffer==NULL);
        sem_init(&buffer->header.write_lock,0,1);
        //printf("[createNewMemoryBuffer] lock initialized\n");
        return buffer;
    }
    union IntegerDecomposer{
        int raw_int;
        char value[4];
    };
    int compareMemBuffer(const void* a,const void* b, void* udata){
        const struct MemoryBuffer* mem_buffer1 = a;
        const struct MemoryBuffer* mem_buffer2 = b;
        return mem_buffer1->header.block_number-mem_buffer2->header.block_number;
    }
    uint64_t fetchMemBufferHash(const void* item,uint64_t seed0,uint64_t seed1){
        const struct MemoryBuffer* mem_buffer = item;
        const int block_number = mem_buffer->header.block_number;
        union IntegerDecomposer decomposer = {block_number};

        return hashmap_sip(decomposer.value,sizeof(decomposer.value),seed0,seed1);
    }
    /**
     * 
     * 새로운 메모리 버퍼 관리 객체를 생성하는 함수
     *      n: 디스크의 버퍼의 총 갯수
     *      manageable_n: 4KB 버퍼가 만약 n=10000개이상이 되면, 실질적으로 RAM에 탑재가 불가능하므로, RAM에 올릴 수 있는 버퍼의 최대 갯수
     *      block_size: 각 메모리 버퍼가 가지는 실질적인 크기(단위는 바이트(B))
     */
    struct MemoryBufferManager* createNewMemoryBufferManager(int n,int manageable_n,long long block_size){
        if(manageable_n>n){
            perror("error: manageable_n>n");
            return NULL;
        }
        struct MemoryBufferManager* manager = NULL;
        manager = (struct MemoryBufferManager*)malloc(sizeof(struct MemoryBufferManager));
        memset(manager,0,sizeof(struct MemoryBufferManager));
        manager->explicit_block_size=block_size;
        manager->total_number_of_memory_buffers = n;
        manager->manageable_number_of_memory_buffers=manageable_n;
        manager->controller = newMemoryBufferController();
        //manager->memory_buffers = (struct MemoryBuffer**)malloc(manageable_n*sizeof(struct MemoryBuffer*));
        manager->memory_buffers = hashmap_new(sizeof(struct MemoryBuffer*),0,0,0,fetchMemBufferHash,compareMemBuffer,NULL,NULL);

        return manager;
    }
    void freeMemoryBufferAt(struct MemoryBufferManager* memory_buffer_wrapper,int block_number,unsigned char free_content){
        if(0<=block_number&&block_number<memory_buffer_wrapper->total_number_of_memory_buffers){
            struct hashmap* memory_buffers = memory_buffer_wrapper->memory_buffers;
            struct MemoryBuffer* mem_buffer = hashmap_get(memory_buffers,block_number);
            hashmap_delete(memory_buffers,mem_buffer);
            if(free_content>0){
                free(mem_buffer->buffer);
                sem_destroy(&mem_buffer->header.write_lock);
                free(mem_buffer);
            }
            /*struct MemoryBuffer* buffer = memory_buffer_wrapper->memory_buffers[index];
            free(buffer->buffer);
            free(memory_buffer_wrapper->memory_buffers[index]);
            sem_destroy(&buffer->header.write_lock);
            memory_buffer_wrapper->memory_buffers[index] = NULL;*/
        }
    }
    void allFreeMemoryBuffer(struct MemoryBufferManager* memory_buffer_wrapper){
        struct hashmap* memory_buffers = memory_buffer_wrapper->memory_buffers;
        //hashmap_clear(memory_buffers,1);
    
    }
    int setMemoryBufferAt(struct MemoryBufferManager* memory_buffer_wrapper,int block_number,char* buffer,long long buffer_length_byte){
        if(0<=block_number&&block_number<memory_buffer_wrapper->total_number_of_memory_buffers){
            struct hashmap* memory_buffers = memory_buffer_wrapper->memory_buffers;
            struct MemoryBuffer* mem_buffer = hashmap_get(memory_buffers,&(struct MemoryBuffer){.header.block_number=block_number});
            if(mem_buffer==NULL){
                mem_buffer = createNewMemoryBuffer(buffer_length_byte,-1);
                //if()
                mem_buffer->header.isDirty = 1;
                memcpy(mem_buffer->buffer,buffer,buffer_length_byte*sizeof(char));
            }
            else{
                sem_wait(&mem_buffer->header.write_lock);
                
                if(mem_buffer->header.block_size_byte!=buffer_length_byte){
                    mem_buffer->header.isDirty = 1;
                    free(mem_buffer->buffer);
                   ( &mem_buffer->header)->block_size_byte = buffer_length_byte;
                    mem_buffer->buffer = (char*)malloc(buffer_length_byte*sizeof(char));
                    memcpy(mem_buffer->buffer,buffer,buffer_length_byte*sizeof(char));
                }
                else{
                    //TODO: IMPLEMENTATION - check equal checksum
                    memcpy(mem_buffer->buffer,buffer,buffer_length_byte*sizeof(char));
                }
                sem_post(&mem_buffer->header.write_lock);
            }
            return 1;
        }
        return 0;
    }
    int putMemoryBufferAt(struct MemoryBufferManager* memory_buffer_wrapper,int block_number,struct MemoryBuffer* mem_buffer){
        if(0<=block_number&&block_number<memory_buffer_wrapper->total_number_of_memory_buffers){
            struct hashmap* memory_buffers = memory_buffer_wrapper->memory_buffers;

            hashmap_set(memory_buffers,mem_buffer);
            return 1;
        }
        return 0;
    }
     struct MemoryBuffer* getMemoryBufferMap(struct MemoryBufferManager* memory_buffer_wrapper,int block_number){
        if(0<=block_number&&block_number<memory_buffer_wrapper->total_number_of_memory_buffers){
            struct hashmap* memory_buffers = memory_buffer_wrapper->memory_buffers;
            struct MemoryBuffer* search_obj = (struct MemoryBuffer*)malloc(sizeof(struct MemoryBuffer));
            memset(search_obj,0,sizeof(struct MemoryBuffer));
            (&search_obj->header)->block_number = block_number;
            struct MemoryBuffer* mem_buffer = hashmap_get(memory_buffers,search_obj);
            free(search_obj);
            printf("[getMemoryBufferMap] result is %sNULL (at %d)\n",mem_buffer!=NULL?"not ":"",block_number);
            return mem_buffer;
        }
        return NULL;
     }
    unsigned char MemControllerCheckContentValidation(struct MemoryBufferController* self,struct MemoryBuffer* memory_buffer,char* buffer){
        struct MD5Controller* hash_controller = self->hash_controller;
        uint8_t* precaluclate_hash = (&memory_buffer->header)->buffer_precalculated_hash;
        uint8_t* calculated_hash =  hash_controller->fetchHashValue(buffer,memory_buffer->header.block_size_byte);
        return hash_controller->isEqualMd5Hash(precaluclate_hash,calculated_hash);
    }
    void MemControllerCheckContentValidationAndSetValidBit(struct MemoryBufferController* self,struct MemoryBuffer* memory_buffer,char* buffer){
        struct MD5Controller* hash_controller = self->hash_controller;
        uint8_t* precaluclate_hash = (&memory_buffer->header)->buffer_precalculated_hash;
        uint8_t* calculated_hash =  hash_controller->fetchHashValue(buffer,memory_buffer->header.block_size_byte);
        unsigned char isSameHash = hash_controller->isEqualMd5Hash(precaluclate_hash,calculated_hash);
        if(!isSameHash){
            hash_controller->copyHashValue(precaluclate_hash,calculated_hash);
        }
    }
    void MemControllerSearchMapMemoryBuffer(struct MemoryBufferController* self,int block_number){

    }
    

     struct MemoryBufferController* newMemoryBufferController(){
        struct MemoryBufferController* controller = (struct MemoryBufferController*) malloc(sizeof(struct MemoryBufferController));
        controller->hash_controller = newMD5Controller();
        controller->checkContentValidation = MemControllerCheckContentValidation;
        controller->checkContentValidationAndSetValidBit = MemControllerCheckContentValidationAndSetValidBit;
        controller->getMemoryBufferMap = getMemoryBufferMap;
        controller->freeMemoryBufferAt = freeMemoryBufferAt;
        controller->allFreeMemoryBuffer = allFreeMemoryBuffer;
        controller->setMemoryBufferAt = setMemoryBufferAt;
        controller->putMemoryBufferAt = putMemoryBufferAt;
        return controller;
    }
    