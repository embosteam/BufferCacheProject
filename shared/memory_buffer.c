#include "memory_buffer.h"
struct MemoryBuffer* createNewMemoryBuffer(int block_size_byte,int block_number){
        struct MemoryBuffer* buffer = NULL;
        buffer = (struct MemoryBuffer*)malloc(sizeof(struct MemoryBuffer));
        memset(buffer,0,sizeof(struct MemoryBuffer));
        memset(&buffer->header,0,sizeof(struct MemoryBufferHeader));
        buffer->buffer = (char*)aligned_alloc(block_size_byte,block_size_byte);//aligned_alloc(block_size,block_size*sizeof(char));
        (buffer->header.block_number) = block_number;
        buffer->header.block_size_byte = block_size_byte;
        
        sem_init(&buffer->header.write_lock,0,1);
        return buffer;
    }
    union IntegerDecomposer{
        int raw_int;
        char value[4];
    };
    struct HashMapWrapper* newHashMapWrapper(int block_number,struct MemoryBuffer* mem_buffer){
        struct HashMapWrapper* wrapper = (struct HashMapWrapper*)malloc(sizeof(struct HashMapWrapper));
        memset(wrapper,0,sizeof(struct HashMapWrapper));
        wrapper->block_number = block_number;
        wrapper->memory_buffer = mem_buffer;
        return wrapper;
    }
    int compareMemBuffer(const void* a,const void* b, void* udata){
        
        const struct HashMapWrapper* mem_buffer_wrapper1 = a;
        const struct HashMapWrapper* mem_buffer_wrapper2 = b;
        return mem_buffer_wrapper1->block_number-mem_buffer_wrapper2->block_number;
    }

    uint64_t fetchMemBufferHash(const void* item,uint64_t seed0,uint64_t seed1){
        const struct HashMapWrapper* mem_buffer = item;
        const int block_number = mem_buffer->block_number;
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
    struct MemoryBufferManager* createNewMemoryBufferManager(int n,int manageable_n,long long block_size,const unsigned char replacement_type){
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
        manager->controller = newMemoryBufferController(replacement_type);
        //manager->memory_buffers = (struct MemoryBuffer**)malloc(manageable_n*sizeof(struct MemoryBuffer*));
        manager->memory_buffers = hashmap_new(sizeof(struct HashMapWrapper),0,0,0,fetchMemBufferHash,compareMemBuffer,NULL,NULL);

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
            
        }
    }
    void allFreeMemoryBuffer(struct MemoryBufferManager* memory_buffer_wrapper){
        struct hashmap* memory_buffers = memory_buffer_wrapper->memory_buffers;
        //hashmap_clear(memory_buffers,1);
    
    }
    int setMemoryBufferAt(struct MemoryBufferManager* memory_buffer_wrapper,int block_number,char* buffer,long long buffer_length_byte){
        if(0<=block_number&&block_number<memory_buffer_wrapper->total_number_of_memory_buffers){
            struct hashmap* memory_buffers = memory_buffer_wrapper->memory_buffers;
            struct MemoryBuffer* mem_buffer = NULL;
             struct HashMapWrapper* wrapper_obj = newHashMapWrapper(mem_buffer->header.block_number,mem_buffer);
            struct HashMapWrapper* hashmap_data_wrapper=  hashmap_get(memory_buffers,wrapper_obj);
            free(wrapper_obj);
            if(hashmap_data_wrapper!=NULL){
                mem_buffer = hashmap_data_wrapper->memory_buffer;
            }
            if(mem_buffer==NULL){
                mem_buffer = createNewMemoryBuffer(buffer_length_byte,block_number);
                //if()
                mem_buffer->header.isDirty = 1;
                memcpy(mem_buffer->buffer,buffer,buffer_length_byte*sizeof(char));
                 struct HashMapWrapper* wrapper_obj = newHashMapWrapper(mem_buffer->header.block_number,mem_buffer);
                hashmap_set(memory_buffers,wrapper_obj);
                free(wrapper_obj);
            }
            else{
                sem_wait(&mem_buffer->header.write_lock);
                //printf("[setMemoryBufferAt] set exist item (%d,%d) - blk:%d\n",mem_buffer!=NULL,mem_buffer!=NULL&&mem_buffer->buffer!=NULL,mem_buffer->header.block_number);
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
                mem_buffer->header.isDirty = true;
                
                sem_post(&mem_buffer->header.write_lock);
            }
            return 1;
        }
        return 0;
    }
    
    
    
    int putMemoryBufferAt(struct MemoryBufferManager* memory_buffer_wrapper,int block_number,struct MemoryBuffer* mem_buffer){
        if(0<=block_number&&block_number<memory_buffer_wrapper->total_number_of_memory_buffers){
            struct hashmap* memory_buffers = memory_buffer_wrapper->memory_buffers;
            struct MemoryBufferController* buffer_controller = memory_buffer_wrapper->controller;
            struct ReplacementPolicyManager* replacement_manager = buffer_controller->replacement_policy_manager;

             struct HashMapWrapper* wrapper_obj = newHashMapWrapper(mem_buffer->header.block_number,mem_buffer);
             
            unsigned long long start_time = getCurrentTime();
            hashmap_set(memory_buffers,wrapper_obj);
            unsigned long long end_time = getCurrentTime();
            memory_buffer_wrapper->pure_hashmap_set_time+=(double)(end_time-start_time)/((double)(CONFIG_RANDOM_WRITE_COUNT+2));
            
            free(wrapper_obj);
            struct MemoryBuffer* memory_buffer = mem_buffer;
            struct HashMapWrapper* search_obj = newHashMapWrapper(mem_buffer->header.block_number,NULL);

            struct HashMapWrapper* mem_buffer_wrapper = hashmap_get(memory_buffers,search_obj);
            free(search_obj);
            mem_buffer = NULL;
            if(mem_buffer_wrapper!=NULL){
                
                mem_buffer = mem_buffer_wrapper->memory_buffer;
                //replacement_manager->updateAccessInfo(replacement_manager,mem_buffer);
                replacement_manager->updateAccessInfoWithThread(replacement_manager,mem_buffer_wrapper->memory_buffer);
            }
            /*printf("[putMemoryBufferAt] check setting new item (%d,%d) - blk:%d\n",mem_buffer!=NULL,mem_buffer!=NULL&&mem_buffer->buffer!=NULL,mem_buffer->header.block_number);
            printf("[putMemoryBufferAt] add check: (from hashmap)%x, (from origin)%x\n",memory_buffer,mem_buffer);
            printf("[putMemoryBufferAt] origin item after finish setting(found:%d)  (%d,%d) - blk:%d , addr: %x\n",mem_buffer_wrapper!=NULL,memory_buffer!=NULL,memory_buffer!=NULL&&memory_buffer->buffer!=NULL,memory_buffer->header.block_number,mem_buffer);
            */
            //printf("[putMemoryBufferAt] origin item after finish setting(found:%d)  (%d,%d) - blk:%d , addr: %x\n",mem_buffer_wrapper!=NULL,memory_buffer!=NULL,memory_buffer!=NULL&&memory_buffer->buffer!=NULL,mem_buffer->header.block_number,mem_buffer);
            
            return 1;
        }
        return 0;
    }

     struct MemoryBuffer* getMemoryBufferMap(struct MemoryBufferManager* memory_buffer_wrapper,int block_number){
        if(0<=block_number&&block_number<memory_buffer_wrapper->total_number_of_memory_buffers){
            struct hashmap* memory_buffers = memory_buffer_wrapper->memory_buffers;
            
            struct HashMapWrapper* search_obj = newHashMapWrapper(block_number,NULL);
            unsigned long long start_time = getCurrentTime();
            struct HashMapWrapper* mem_buffer_wrapper = hashmap_get(memory_buffers,search_obj);
            unsigned long long end_time = getCurrentTime();
            memory_buffer_wrapper->pure_hashmap_get_time+=(double)(end_time-start_time)/((double)(CONFIG_RANDOM_ACCESS_SEQUENCE_ELEMENT_COUNT+CONFIG_RANDOM_WRITE_COUNT+2));
            
            struct MemoryBufferController* buffer_controller = memory_buffer_wrapper->controller;
            struct ReplacementPolicyManager* replacement_manager = buffer_controller->replacement_policy_manager;
            free(search_obj);
            if(mem_buffer_wrapper!=NULL){
                //printf("[getMemoryBufferMap] start updateAccessInfo\n");
                //replacement_manager->updateAccessInfo(replacement_manager,mem_buffer_wrapper->memory_buffer);
                //replacement_manager->updateAccessInfoWithThread(replacement_manager,mem_buffer_wrapper->memory_buffer);
                
                
                //printf("[getMemoryBufferMap] finish updateAccessInfo\n");
                struct MemoryBuffer* mem_buffer = mem_buffer_wrapper->memory_buffer;
                return mem_buffer;
            }
            
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
    /**
    * 제거될 버퍼를 찾아야 하는 지 확인하는 함수
    */
    int shouldFindEvictableBuffer(struct MemoryBufferController* self,struct MemoryBufferManager* manager){
        size_t memory_buffer_count = hashmap_count(manager->memory_buffers);
        return manager->manageable_number_of_memory_buffers<=memory_buffer_count;
    }
    /**
    * 여러 캐시 정책을 통해 제거될 버퍼를 찾는 함수
    */
    struct MemoryBuffer* findEvictableBuffer(struct MemoryBufferController* self,struct MemoryBufferManager* manager){
        struct MemoryBufferController* buffer_controller = manager->controller;
        struct ReplacementPolicyManager* replacement_manager = buffer_controller->replacement_policy_manager;
        /*if(CONFIG_DEBUG_MODE){
         replacement_manager->debugPolicyContent(replacement_manager,0);
          replacement_manager->debugPolicyContent(replacement_manager,1);
           replacement_manager->debugPolicyContent(replacement_manager,2);
        }*/
        struct DoublyLinkedDequeInternalNode* evictable_internal_node = replacement_manager->getEvictableNode(replacement_manager,replacement_manager->primary_policy_type);
        struct MemoryBuffer* return_value = NULL;
        if(evictable_internal_node!=NULL){
            struct MemoryBuffer* hashmap_wrapper = evictable_internal_node->value;
            return_value = hashmap_wrapper;//->memory_buffer;
        }
        return return_value;
    }
    /**
    * 메모리 버퍼를 해시맵에서 제거하는 함수
    */
    int deleteMemoryBuffer(struct MemoryBufferController* self,struct MemoryBufferManager* manager,struct MemoryBuffer* mem_buffer){
        struct hashmap* map = manager->memory_buffers;
        struct MemoryBufferController* buffer_controller = manager->controller;
        struct ReplacementPolicyManager* replacement_manager = buffer_controller->replacement_policy_manager;
        if(mem_buffer!=NULL){
            struct HashMapWrapper* search_obj = newHashMapWrapper(mem_buffer->header.block_number,mem_buffer);
            struct HashMapWrapper* mem_buffer_wrapper = hashmap_get(map,search_obj);
            if(mem_buffer_wrapper!=NULL){
                struct MemoryBuffer* vanishable_mem_buffer = mem_buffer_wrapper->memory_buffer;
                //printf("[deleteMemoryBuffer] waiting...\n");
                /*switch(CONFIG_EVICTION_FLUSH_TYPE){
                    case EVICTION_FLUSH_TYPE_SINGLE_FLUSH_THREAD:{
                        while(vanishable_mem_buffer->header.isBufferBeingWrittenNow == 1){
                            usleep(1);
                        }
                        break;
                    }
                    default: break;
                }*/
                if(CONFIG_DEBUG_MODE){
                                printf("[deleteMemoryBuffer] waiting...\n");
                            }
                //printf("[deleteMemoryBuffer] waiting...\n");
                while(1){
                    //sem_wait(&vanishable_mem_buffer->header.write_lock);

                    if(!vanishable_mem_buffer->header.isBufferBeingWrittenNow && !vanishable_mem_buffer->header.request_write_count ){
                        //vanishable_mem_buffer->header.isBufferBeingWrittenNow = 1;
                        //sem_post(&vanishable_mem_buffer->header.write_lock);
                        break;
                    }
                    else{
                        //sem_post(&vanishable_mem_buffer->header.write_lock);
                        usleep(5);
                    }
                    
                }
                if(CONFIG_DEBUG_MODE){
                    printf("[deleteMemoryBuffer] keep going\n");
                }
                //printf("[deleteMemoryBuffer] keep going\n");
                
                sem_wait(&vanishable_mem_buffer->header.write_lock);
                unsigned char isMemDirty = vanishable_mem_buffer->header.isDirty;
                /*if(isMemDirty){
                    vanishable_mem_buffer->header.isBufferBeingWrittenNow=1;
                }*/
                sem_post(&vanishable_mem_buffer->header.write_lock);

                if(isMemDirty){
                    
                    switch(CONFIG_EVICTION_FLUSH_TYPE){
                        case(EVICTION_FLUSH_TYPE_SINGLE_FLUSH_THREAD):{
                            vanishable_mem_buffer->header.isBufferBeingWrittenNow=1;
                           // printf("[deleteMemoryBuffer] blk(%d) setting isBufferBeingWrittenNow\n",vanishable_mem_buffer->header.block_number);
                            #if(USE_MMAP_RW==0)
                            pthread_t flush_thread_id = flushBufferWithThread(manager->disk_fd,vanishable_mem_buffer);
                            #else
                            pthread_t flush_thread_id = flushBufferWithThread2(manager->disk_buffer,vanishable_mem_buffer);
                            #endif                            
                            long long thread_result;
                            //printf("\t[deleteMemoryBuffer] thread joining... %d\n",flush_thread_id);
                            pthread_join(flush_thread_id,&thread_result);
                            
                            vanishable_mem_buffer->header.isBufferBeingWrittenNow=0;
                            break;
                        }
                        case(EVICTION_FLUSH_TYPE_THREAD_POOL):{
                            #if(USE_MMAP_RW==0)
                            manager->tp->addQueue(manager->tp,manager->disk_fd,vanishable_mem_buffer,1);
                            #else
                            manager->tp->addQueue2(manager->tp,manager->disk_buffer,vanishable_mem_buffer,1);
                            #endif
                            if(CONFIG_DEBUG_MODE){
                                printf("[deleteMemoryBuffer] waiting...\n");
                            }
                            while(1){//vanishable_mem_buffer->header.isBufferBeingWrittenNow == 1){
                                //sem_wait(&vanishable_mem_buffer->header.write_lock);
                                unsigned char isMemDirty2 = vanishable_mem_buffer->header.isDirty;
                                //sem_post(&vanishable_mem_buffer->header.write_lock);
                                if(!isMemDirty2){
                                    break;
                                }
                                usleep(1);
                            }
                            if(CONFIG_DEBUG_MODE){
                                printf("[deleteMemoryBuffer] keep going\n");
                            }
                            break;
                        }
                        default:break;
                    }
                }
                

                //sem_post(&vanishable_mem_buffer->header.write_lock);
                sem_wait(&vanishable_mem_buffer->header.write_lock);
                (&vanishable_mem_buffer->header)->isValid = 0;
                free(vanishable_mem_buffer->buffer);
                vanishable_mem_buffer->buffer=NULL;
                sem_post(&vanishable_mem_buffer->header.write_lock);
                sem_destroy(&vanishable_mem_buffer->header.write_lock);
                memset(&vanishable_mem_buffer->header.write_lock,0,sizeof(sem_t));
                replacement_manager->deleteAt(replacement_manager,mem_buffer_wrapper->memory_buffer);
                
            }
            hashmap_delete(map,search_obj);
            free(search_obj);
            return 1;
        }
        

        return 0;
    }

     struct MemoryBufferController* newMemoryBufferController(const unsigned char replacement_type){
        struct MemoryBufferController* controller = (struct MemoryBufferController*) malloc(sizeof(struct MemoryBufferController));
        controller->hash_controller = newMD5Controller();
        controller->replacement_policy_manager = newReplacementPolicyManager(replacement_type, 1);
        controller->checkContentValidation = MemControllerCheckContentValidation;
        controller->checkContentValidationAndSetValidBit = MemControllerCheckContentValidationAndSetValidBit;
        controller->getMemoryBufferMap = getMemoryBufferMap;
        controller->freeMemoryBufferAt = freeMemoryBufferAt;
        controller->allFreeMemoryBuffer = allFreeMemoryBuffer;
        controller->setMemoryBufferAt = setMemoryBufferAt;
        controller->putMemoryBufferAt = putMemoryBufferAt;
        controller->deleteMemoryBuffer = deleteMemoryBuffer;
        controller->findEvictableBuffer = findEvictableBuffer;
        controller->shouldFindEvictableBuffer = shouldFindEvictableBuffer;
        return controller;
    }
    