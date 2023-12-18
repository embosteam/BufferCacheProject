#include "replacement_policy.h"

 void FIFOPolicy_updateAccessInfo(struct FIFOPolicy* self,struct MemoryBuffer* hash_wrapper){
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }
        
        struct DoublyLinkedDequeInternalNode* fifo_node = hash_wrapper->header.fifo_node;
        struct DoublyLinkedDeque* dq = self->deque;
        if(fifo_node!=NULL){
            struct DoublyLinkedDequeInternalNode* last_node = dq->getLast(dq);
            dq->popAt(dq,fifo_node);
            free(fifo_node);
            (&hash_wrapper->header)->fifo_node = dq->pushLast(dq,hash_wrapper);
            //dq->switchNode(dq,fifo_node,last_node);
        }else{
            (&hash_wrapper->header)->fifo_node = dq->pushLast(dq,hash_wrapper);
        }
         if(self->isLockSupported){
            sem_post(&self->lock);
        }
    }
    struct DoublyLinkedDequeInternalNode* FIFOPolicy_getEvictableNode(struct FIFOPolicy* self){
        if(self->isLockSupported){
            sem_wait(&self->lock);
        }
        struct DoublyLinkedDequeInternalNode* result =  self->deque->getFirst(self->deque);
         if(self->isLockSupported){
            sem_post(&self->lock);
        }
        return result;
    }

    void FIFOPolicy_deleteAt(struct FIFOPolicy* self,struct MemoryBuffer* hash_wrapper){
        if(self->isLockSupported){
            sem_wait(&self->lock);
        }
        struct DoublyLinkedDequeInternalNode* fifo_node = hash_wrapper->header.fifo_node;
        struct DoublyLinkedDeque* dq = self->deque;
        if(fifo_node!=NULL){
            dq->popAt(dq,fifo_node);
            fifo_node = dq->getFirst(dq);
            (&hash_wrapper->header)->fifo_node = fifo_node;
        }
         if(self->isLockSupported){
            sem_post(&self->lock);
        }
    }

    void LFUPolicy_updateAccessInfo(struct LFUPolicy* self,struct MemoryBuffer* hash_wrapper){
        if(self->isLockSupported){
            sem_wait(&self->lock);
        }
        //printf("[LFUPolicy_updateAccessInfo] start %x\n",hash_wrapper);
        struct DoublyLinkedDequeInternalNode* fifo_node = hash_wrapper->header.lfu_node;
        struct DoublyLinkedDeque* dq = self->deque;
        //printf("[LFUPolicy_updateAccessInfo] start add count\n");
        hash_wrapper->header.access_count ++;
        if(fifo_node!=NULL){
            //printf("[LFUPolicy_updateAccessInfo] start moving\n");
            const char isFirst = dq->getFirst(dq)==fifo_node;
            struct DoublyLinkedDequeInternalNode* next_node = fifo_node->next;
            const unsigned int fifo_access_count = hash_wrapper->header.access_count;
            //printf("[LFUPolicy_updateAccessInfo] middle moving\n");

            while(next_node!=NULL){
                //printf("next node: %x (%x,%x) (fifon: %x , first: %x , last: %x)\n",
                //next_node,next_node->previous,next_node->next,
                //fifo_node,dq->getFirst(dq),dq->getLast(dq));
                struct MemoryBuffer* next_hashmap_wrapper = next_node->value;
                const unsigned int next_node_count = next_hashmap_wrapper->header.access_count;
                if(fifo_access_count>=next_node_count){
                    dq->switchNode(dq,fifo_node,next_node);
                    //next_node = next_node->next;//fifo_node->next;
                    next_node = fifo_node->next;
                    
                }
                else{
                    break;
                }
            }
            //(&hash_wrapper->header)->lfu_node = dq->getFirst(dq);
            
            //printf("[LFUPolicy_updateAccessInfo] finish moving\n");
        }
        else{
            (&hash_wrapper->header)->lfu_node =dq->pushFirst(dq,hash_wrapper);
            struct DoublyLinkedDequeInternalNode* fifo_node = hash_wrapper->header.lfu_node;
            struct DoublyLinkedDequeInternalNode* next_node = fifo_node->next;
            const unsigned int fifo_access_count = hash_wrapper->header.access_count;
            //printf("[LFUPolicy_updateAccessInfo] middle moving\n");

            while(next_node!=NULL){
                //printf("next node: %x (%x,%x) (fifon: %x , first: %x , last: %x)\n",
                //next_node,next_node->previous,next_node->next,
                //fifo_node,dq->getFirst(dq),dq->getLast(dq));
                struct MemoryBuffer* next_hashmap_wrapper = next_node->value;
                const unsigned int next_node_count = next_hashmap_wrapper->header.access_count;
                if(fifo_access_count>=next_node_count){
                    dq->switchNode(dq,fifo_node,next_node);
                    //next_node = next_node->next;//fifo_node->next;
                    next_node = fifo_node->next;
                    
                }
                else{
                    break;
                }
            }
        }
         if(self->isLockSupported){
            sem_post(&self->lock);
        }
        //printf("[LFUPolicy_updateAccessInfo] finish\n");
    }
    struct DoublyLinkedDequeInternalNode* LFUPolicy_getEvictableNode(struct LFUPolicy* self){
        if(self->isLockSupported){
            sem_wait(&self->lock);
        }
        struct DoublyLinkedDequeInternalNode* value = self->deque->getFirst(self->deque);
         if(self->isLockSupported){
            sem_post(&self->lock);
        }
        return value;
    }

    void LFUPolicy_deleteAt(struct LFUPolicy* self,struct MemoryBuffer* hash_wrapper){
        if(self->isLockSupported){
            sem_wait(&self->lock);
        }
        struct DoublyLinkedDequeInternalNode* fifo_node = hash_wrapper->header.lfu_node;
        struct DoublyLinkedDeque* dq = self->deque;
        if(fifo_node!=NULL){
            dq->popAt(dq,fifo_node);
            //fifo_node = dq->getFirst(dq);
            //(&hash_wrapper->header)->lfu_node = fifo_node;
        }
         if(self->isLockSupported){
            sem_post(&self->lock);
        }
    }

    
     void LRUPolicy_updateAccessInfo(struct LRUPolicy* self,struct MemoryBuffer* hash_wrapper){
        if(self->isLockSupported){
            sem_wait(&self->lock);
        }
        struct DoublyLinkedDequeInternalNode* fifo_node = hash_wrapper->header.lru_node;
        struct DoublyLinkedDeque* dq = self->deque;

        (&hash_wrapper->header)->lastest_accessing_timestamp = getCurrentTime();
        if(fifo_node!=NULL){
            struct DoublyLinkedDequeInternalNode* next_node = fifo_node->next;
            const unsigned int fifo_access_count = hash_wrapper->header.lastest_accessing_timestamp;
            while(next_node!=NULL){
                struct MemoryBuffer* next_hashmap_wrapper = next_node->value;
                const unsigned int next_node_count = next_hashmap_wrapper->header.lastest_accessing_timestamp;
                if(fifo_access_count>=next_node_count){
                    dq->switchNode(dq,fifo_node,next_node);
                    next_node = fifo_node->next;
                    //next_node = next_node->next;
                }
                else{
                    break;
                }
            }
            //(&hash_wrapper->header)->lru_node = dq->getFirst(dq);
        }
        else{
            (&hash_wrapper->header)->lru_node = dq->pushLast(dq,hash_wrapper);
            //(&hash_wrapper->header)->lru_node =dq->getFirst(dq);
        }
         if(self->isLockSupported){
            sem_post(&self->lock);
        }
    }
    struct DoublyLinkedDequeInternalNode* LRUPolicy_getEvictableNode(struct LRUPolicy* self){
        if(self->isLockSupported){
            sem_wait(&self->lock);
        }
        struct DoublyLinkedDequeInternalNode* value = self->deque->getFirst(self->deque);
        if(self->isLockSupported){
            sem_post(&self->lock);
        }
        return value;
    }

    void LRUPolicy_deleteAt(struct LRUPolicy* self,struct MemoryBuffer* hash_wrapper){
        if(self->isLockSupported){
            sem_wait(&self->lock);
        }
        struct DoublyLinkedDequeInternalNode* fifo_node = hash_wrapper->header.lru_node;
        struct DoublyLinkedDeque* dq = self->deque;
        if(fifo_node!=NULL){
            dq->popAt(dq,fifo_node);
            struct DoublyLinkedDequeInternalNode* fifo_node2 = fifo_node;
            fifo_node = dq->getFirst(dq);
            //printf("  [LRUPolicy_deleteAt] fifon: %x fifon->next: %x,fifon2: %x\n",fifo_node2,fifo_node2->next,fifo_node);
            (&hash_wrapper->header)->lru_node = fifo_node;
        }
        else{
           // printf("  [LRUPolicy_deleteAt] lru node is NULL\n");
        }
         if(self->isLockSupported){
            sem_post(&self->lock);
        }
    }


    struct FIFOPolicy* newFIFOPolicy(unsigned int concurrent_count){
        struct FIFOPolicy* self = (struct FIFOPolicy*)malloc(sizeof(struct FIFOPolicy));
        memset(self,0,sizeof(struct FIFOPolicy));
        self->isLockSupported = concurrent_count>0;
        if(self->isLockSupported){
            sem_init(&self->lock,0,concurrent_count);
        }
        self->deque = newDoublyLinkedDeque(concurrent_count);
        self->updateAccessInfo = FIFOPolicy_updateAccessInfo;
        self->getEvictableNode = FIFOPolicy_getEvictableNode;
        self->deleteAt = FIFOPolicy_deleteAt;
        return self;
    }

    struct LFUPolicy* newLFUPolicy(unsigned int concurrent_count){
        struct LFUPolicy* self = (struct LFUPolicy*)malloc(sizeof(struct LFUPolicy));
        memset(self,0,sizeof(struct LFUPolicy));
        self->isLockSupported = concurrent_count>0;
        if(self->isLockSupported){
            sem_init(&self->lock,0,concurrent_count);
        }
        self->deque = newDoublyLinkedDeque(concurrent_count);
        self->updateAccessInfo = LFUPolicy_updateAccessInfo;
        self->getEvictableNode = LFUPolicy_getEvictableNode;
        self->deleteAt = LFUPolicy_deleteAt;
        return self;
    }

    struct LRUPolicy* newLRUPolicy(unsigned int concurrent_count){
        struct LRUPolicy* self = (struct LRUPolicy*)malloc(sizeof(struct LRUPolicy));
        memset(self,0,sizeof(struct LRUPolicy));
        self->isLockSupported = concurrent_count>0;
        if(self->isLockSupported){
            sem_init(&self->lock,0,concurrent_count);
        }
        self->deque = newDoublyLinkedDeque(concurrent_count);
        self->updateAccessInfo = LRUPolicy_updateAccessInfo;
        self->getEvictableNode = LRUPolicy_getEvictableNode;
        self->deleteAt = LRUPolicy_deleteAt;
        return self;
    }

    void ReplacementPolicyManager_updateAccessInfo(struct ReplacementPolicyManager* self,struct MemoryBuffer* hashmap_wrapper){
         switch(self->primary_policy_type){
            case REPLACEMENT_POLICY_TYPE_FIFO:{
                if(self->fifo!=NULL){
                    self->fifo->updateAccessInfo(self->fifo,hashmap_wrapper);
                }
                break;
            }
            case REPLACEMENT_POLICY_TYPE_LRU:{
                if(self->lru!=NULL){
                    self->lru->updateAccessInfo(self->lru,hashmap_wrapper);
                }
                break;
            }
            case REPLACEMENT_POLICY_TYPE_LFU:{
                if(self->lfu!=NULL){
                    self->lfu->updateAccessInfo(self->lfu,hashmap_wrapper);
                }
                break;
            }
            default:break;
         }
    }
    struct DoublyLinkedDequeInternalNode* ReplacementPolicyManager_getEvictableNode(struct ReplacementPolicyManager* self,int policy_type){
        switch(policy_type){
            case REPLACEMENT_POLICY_TYPE_FIFO:{
                struct DoublyLinkedDequeInternalNode* internal_node = self->fifo->getEvictableNode(self->fifo);
                return internal_node;
            }
            case REPLACEMENT_POLICY_TYPE_LFU:{
                return self->lfu->getEvictableNode(self->lfu);
            }
            default:{
                return self->lru->getEvictableNode(self->lru);
            }
        }
    }
    void ReplacementPolicyManager_deleteAt(struct ReplacementPolicyManager* self,struct MemoryBuffer* hashmap_wrapper){
        switch(self->primary_policy_type){
            case REPLACEMENT_POLICY_TYPE_FIFO:{
                if(self->fifo!=NULL){
                    self->fifo->deleteAt(self->fifo,hashmap_wrapper);
                }
                break;
            }
            case REPLACEMENT_POLICY_TYPE_LRU:{
                if(self->lru!=NULL){
                    self->lru->deleteAt(self->lru,hashmap_wrapper);
                } 
                break;  
            }
            case REPLACEMENT_POLICY_TYPE_LFU:{
                if(self->lfu!=NULL){
                    self->lfu->deleteAt(self->lfu,hashmap_wrapper);
                }
                break;
            }
            default: break;
        }
    }
    void ReplacementPolicyManager_debugPolicyContent(struct ReplacementPolicyManager* self,int policy_type){
         switch(policy_type){
            case 0:{
                struct FIFOPolicy* policy = self->fifo;
                struct DoublyLinkedDeque* dq = policy->deque;
                struct DoublyLinkedDequeInternalNode* node = dq->first;
                printf("[ReplacementPolicyManager_debugPolicyContent] deque count: %llu\n",dq->getCount(dq));
                printf("[ReplacementPolicyManager_debugPolicyContent - fifo]\n\n");
                struct DoublyLinkedDequeInternalNode* first = dq->getFirst(dq);
                if(first!=NULL){
                    struct MemoryBuffer* first_wrapper = first->value;
                    printf("\t[first] addr=%x blk=%d, access_count: %d mem_buffer: %x\n",
                    first_wrapper,
                    first_wrapper->header.block_number,
                    first_wrapper->header.access_count,
                    first_wrapper->header.block_size_byte);
                }
                else{
                    printf("\t[first] getFirst returns NULL\n");
                }
                while(node!=NULL){
                    struct MemoryBuffer* map_wrapper = node->value;
                    printf("\taddr: %x, value-addr=%x [blk %d] access_count: %d , mem_buffer: %x\n",
                    node,map_wrapper,map_wrapper->header.block_number,map_wrapper->header.access_count,map_wrapper->header.block_size_byte);
                    node= node->next;
                }
                printf("\n");
                break;
            }
            case 1:{
                struct LFUPolicy* policy = self->lfu;
                struct DoublyLinkedDeque* dq = policy->deque;
                struct DoublyLinkedDequeInternalNode* node = dq->first;
                printf("[ReplacementPolicyManager_debugPolicyContent] deque count: %llu\n",dq->getCount(dq));
                printf("[ReplacementPolicyManager_debugPolicyContent - lfu]\n\n");
                struct DoublyLinkedDequeInternalNode* first = dq->getFirst(dq);
                if(first!=NULL){
                    struct MemoryBuffer* first_wrapper = first->value;
                    printf("\t[first] addr=%x blk=%d, access_count: %d mem_buffer: %x\n",
                    first_wrapper,
                    first_wrapper->header.block_number,
                    first_wrapper->header.access_count,
                    first_wrapper->header.block_size_byte);
                }
                else{
                    printf("\t[first] getFirst returns NULL\n");
                }
                while(node!=NULL){
                    struct MemoryBuffer* map_wrapper = node->value;
                    printf("\taddr: %x, value-addr=%x [blk %d] access_count: %d , mem_buffer: %x\n",
                    node,map_wrapper,map_wrapper->header.block_number,map_wrapper->header.access_count,map_wrapper->header.block_size_byte);
                    node= node->next;
                   
                }
                printf("\n");
                break;
            }
            default:{
                struct LRUPolicy* policy = self->lru;
                struct DoublyLinkedDeque* dq = policy->deque;
                struct DoublyLinkedDequeInternalNode* node = dq->first;
                printf("[ReplacementPolicyManager_debugPolicyContent] deque count: %llu\n",dq->getCount(dq));
                printf("[ReplacementPolicyManager_debugPolicyContent - lru]\n\n");
                struct DoublyLinkedDequeInternalNode* first = dq->getFirst(dq);
                if(first!=NULL){
                    struct MemoryBuffer* first_wrapper = first->value;
                    printf("\t[first] addr=%x blk=%d, access_count: %d mem_buffer: %x\n",
                    first_wrapper,
                    first_wrapper->header.block_number,
                    first_wrapper->header.access_count,
                    first_wrapper->header.block_size_byte);
                }
                else{
                    printf("\t[first] getFirst returns NULL\n");
                }
                while(node!=NULL){
                    struct MemoryBuffer* map_wrapper = node->value;
                    printf("\taddr: %x, value-addr=%x [blk %d] access_count: %d , mem_buffer: %x\n access time: %llu",
                    node,map_wrapper,map_wrapper->header.block_number,
                    map_wrapper->header.access_count,map_wrapper->header.block_size_byte,
                    map_wrapper->header.lastest_accessing_timestamp
                    );
                    node= node->next;
                }
                printf("\n");
                break;
            }
        }
    }
    struct UpdateAccessInfoWithThreadParam{
        struct ReplacementPolicyManager* self;
        struct MemoryBuffer* hashmap_wrapper;
    };
    void updateAccessInfoWithThreadMiddleFunc(void* args){
        struct UpdateAccessInfoWithThreadParam* params = args;
        struct ReplacementPolicyManager* self = params->self;
        struct MemoryBuffer* hashmap_wrapper = params->hashmap_wrapper;
        free(params);
        self->updateAccessInfo(self,hashmap_wrapper);
    }
    pthread_t updateAccessInfoWithThread(struct ReplacementPolicyManager* self,struct MemoryBuffer* hashmap_wrapper){
        pthread_t value;
        struct UpdateAccessInfoWithThreadParam* params = (struct UpdateAccessInfoWithThreadParam*)malloc(sizeof(struct UpdateAccessInfoWithThreadParam));
        params->self = self;
        params->hashmap_wrapper = hashmap_wrapper;
        if(pthread_create(&value,NULL/*&attr*/,updateAccessInfoWithThreadMiddleFunc,params)<0){
            perror("[ReplacementPolicyManager::updateAccessInfoWithThread] can not create thread\n");
            self->updateAccessInfo(self,hashmap_wrapper);
            return 0;
        }
        return value;
    }
    struct ReplacementPolicyManager* newReplacementPolicyManager(unsigned char primary_policy_type,unsigned int concurrent_count){
        struct ReplacementPolicyManager* self = (struct ReplacementPolicyManager*)malloc(sizeof(struct ReplacementPolicyManager));
        memset(self,0,sizeof(struct ReplacementPolicyManager));
        self->primary_policy_type = primary_policy_type;
        self->fifo = newFIFOPolicy(concurrent_count);
        self->lfu = newLFUPolicy(concurrent_count);
        self->lru = newLRUPolicy(concurrent_count);
        self->updateAccessInfo = ReplacementPolicyManager_updateAccessInfo;
        self->updateAccessInfoWithThread = updateAccessInfoWithThread;
        self->getEvictableNode = ReplacementPolicyManager_getEvictableNode;
        self->deleteAt = ReplacementPolicyManager_deleteAt;
        self->debugPolicyContent = ReplacementPolicyManager_debugPolicyContent;
        return self;
    }