#include "linked_heap.h"

int defaultComparator(struct LHeap* self,void* s1,void* s2){
        if(s1!=NULL&&s2!=NULL){
            const int v1 = self->fetchKeyFromValue(self,(( struct LHeapInternalNode*)s1)->value);
            const int v2 = self->fetchKeyFromValue(self,(( struct LHeapInternalNode*)s2)->value);
            return v1-v2;
        }
        else if(s1!=NULL){
            return 1;
        }
        else if(s2!=NULL){
            return -1;
        }
        else{
            return 0;
        }
    }
    struct LHeapInternalNode* newLHeapInternalNode(unsigned int index,void* value){
        struct LHeapInternalNode* internal_node = (struct LHeapInternalNode*)malloc(sizeof(struct LHeapInternalNode));
        memset(internal_node,0,sizeof(struct LHeapInternalNode));
        internal_node->index = index;
        internal_node->value = value;
        internal_node->left = NULL;
        internal_node->right = NULL;
        internal_node->left = NULL;
        return internal_node;
    }
    unsigned int LHeapGetCount(struct LHeap* self){
        if(self->isLockSupported){
            sem_wait(&self->lock);
        }
        const unsigned int value = self->count;
        if(self->isLockSupported){
            sem_post(&self->lock);
        }
        return value;
    }
    struct LHeapInternalNode* findParentNode(struct LHeap* self, struct LHeapInternalNode* node,unsigned int value){
        //unsigned int stack[128] = {0,};
        unsigned int* stack = (unsigned int*)malloc(sizeof(unsigned int)*256);
        unsigned int dummy=0;
        int stack_count = 0;
        const unsigned int parent_value = value/2;
        unsigned int search_value = parent_value; 
        stack_count = 0;
        while(search_value>=node->index){
            stack[stack_count++] = search_value;
            search_value /= 2;
        }
        struct LHeapInternalNode* search_node = node;
        stack_count--;
        while(stack_count>0&& search_node!=NULL){
            unsigned int next_value = stack[--stack_count];
            if(search_node->left!=NULL&&search_node->left->index==next_value){
                search_node = search_node->left;
                continue;
            }
            else if(search_node->right!=NULL&&search_node->right->index==next_value){
                search_node = search_node->right;
                continue;
            }
            else{
                
            }
            
        }
        free(stack);
        return search_node;
    }
    struct LHeapInternalNode* findNodeByIndex(struct LHeapInternalNode* node,unsigned int value){
        unsigned int stack[128] = {0,};
        int stack_count = 0;
        const unsigned int parent_value = value;
        unsigned int search_value = parent_value; 
        while(search_value>=node->index){
            stack[stack_count++] = search_value;
            search_value /= 2;
        }
        struct LHeapInternalNode* search_node = node;
        stack_count--;
        while(stack_count>0&& search_node!=NULL){
            unsigned int next_value = stack[--stack_count];
            if(search_node->left!=NULL&&search_node->left->index==next_value){
                search_node = search_node->left;
                continue;
            }
            else if(search_node->right!=NULL&&search_node->right->index==next_value){
                search_node = search_node->right;
                continue;
            }
            else{
                
            }
            
        }
        return search_node;
    }
    int LHeapPush(struct LHeap* self,void* value){
        struct LHeapInternalNode* internal_node =  newLHeapInternalNode(self->count+1,value);
        if(self->isLockSupported){
            sem_wait(&self->lock);
        }
        if(self->root==NULL){
            self->root = internal_node;
            self->count+=1;
            if(self->isLockSupported){
                sem_post(&self->lock);
            }
            return 1;
        }
        else{
            struct LHeapInternalNode* parent_node = findParentNode(self,self->root,self->count+1); //findNodeByIndex(self->root,(self->count+1)/2);
            if(parent_node!=NULL){//완전이진트리의 마지막 위치의 부모 노드를 찾아 자식노드로 추가하고 값이 오름차순이면 상위노드로 값을 이동
                internal_node->parent = parent_node;
                if(internal_node->index==parent_node->index*2){//부모노드의 왼쪽
                    parent_node->left = internal_node;
                    
                }
                else{
                    parent_node->right = internal_node;
                }
                
                self->count++;
                struct LHeapInternalNode* current_node = internal_node;
                int final_updated = 0;
                while(current_node!=NULL){
                    struct LHeapInternalNode* current_parent_node = current_node->parent;
                    if(current_parent_node==NULL){//root 노드
                        final_updated = 1;
                        break;
                    }
                    int compare_result = self->comparator(self,current_node,current_parent_node);
                    if(compare_result>0){//current 노드 값이 더 큰 경우(상대 하위 노드)
                        //break;
                        current_node = current_parent_node;
                    }
                    else{//current 노드 값이 더 작거나 같은 경우
                        void* current_node_value = current_node->value;
                        current_node->value = current_parent_node->value;
                        current_parent_node->value = current_node_value;
                        current_node = current_parent_node;
                    }
                }
                final_updated=1;
                
                if(self->isLockSupported){
                    sem_post(&self->lock);
                }
                
                
                return final_updated;
            }
            else{
                if(self->isLockSupported){
                    sem_post(&self->lock);
                }
                return -1;
            }
        }
    }
    void debugTrav(struct LHeapInternalNode* node){
        if(node!=NULL){
            printf("%d ",node->index);
            debugTrav(node->left);
            printf("\n\t (right of %d) ",node->index);
            debugTrav(node->right);
        }

    }
    void* LHeapGet(struct LHeap* self){
        void* value = NULL;
        if(self->isLockSupported){
            sem_wait(&self->lock);
        }
        if(self->count>0 && self->root!=NULL){
            
            value = self->root->value;
        }
        if(self->isLockSupported){
            sem_post(&self->lock);
        }
        return value;
    }
    unsigned char LHeapIsEmpty(struct LHeap* self){
        unsigned char value = 1;
        if(self->isLockSupported){
            sem_wait(&self->lock);
        }
        if(self->count>0 && self->root!=NULL){
            value = 0;
        }
        if(self->isLockSupported){
            sem_post(&self->lock);
        }
        return value;
    }
    
    unsigned char LHeapPop(struct LHeap* self){
        unsigned char func_value = 0;
        if(self->isLockSupported){
            sem_wait(&self->lock);
        }
        if(self->count==1){
            self->count--;
            self->root->value = NULL;
            struct LHeapInternalNode* last_node = self->root;
            self->root = NULL;
            free(last_node);
            func_value = 1;
        }
        else if(self->count==0){
            func_value = 1;
        }
        else{
                struct LHeapInternalNode* last_node = findNodeByIndex(self->root,self->count);
                if(last_node!=NULL){//last node의 값을 root node로 옮기고 last노드는 tree에서 삭제
                    struct LHeapInternalNode* last_node_parent = last_node->parent;
                    self->root->value = last_node->value;
                    if(last_node_parent!=NULL){
                        if(last_node_parent->left==last_node){
                            last_node_parent->left = NULL;
                        }
                        else{
                            last_node_parent->right = NULL;
                        }
                    }
                    else{//last node가 root일경우, 즉 count=1일 경우
                        self->root = NULL;
                        
                    }


                    free(last_node);
                    
                    func_value = 1;
                    self->count--;
                }
                
                struct LHeapInternalNode* current_node = self->root;

                while(current_node!=NULL){
                   int compare_result =  self->comparator(self, current_node->left,current_node->right);
                   struct LHeapInternalNode* next_node = NULL;
                   if(compare_result<0){//right가 더큼
                        next_node = current_node->left;
                        
                   }
                   else{//left가 더큼
                        next_node = current_node->right;
                        
                   }

                   compare_result = self->comparator(self,next_node,current_node);
                    if(compare_result<0){//current node가 더큼
                        if(next_node!=NULL&&current_node!=NULL){
                            void* current_node_value = current_node->value;
                            current_node->value = next_node->value;
                            next_node->value = current_node_value;
                        }
                        current_node = next_node;
                    }
                    else{//next_node가 더 작거나 같음
                        current_node = next_node;
                    }

                  
                   
                }
        }
        if(self->isLockSupported){
            sem_post(&self->lock);
        }
        return func_value;
    }
    void freeAllInternalNodesExceptRootNode(struct LHeap* self, struct LHeapInternalNode* node){
        if(node!=NULL){

            if(node->left!=NULL){
                freeAllInternalNodesExceptRootNode(self,node->left);
                node->left->value = NULL;
                free(node->left);
                node->left = NULL;
            }
            if(node->right!=NULL){
                freeAllInternalNodesExceptRootNode(self,node->right);
                node->right->value = NULL;
                free(node->right);
                node->right = NULL;
            }
        }
    }
    unsigned char LHeapClear(struct LHeap* self){
        if(self->isLockSupported){
            sem_wait(&self->lock);
        }
        freeAllInternalNodesExceptRootNode(self,self->root);
        if(self->root!=NULL){
            self->root->value = NULL;
            free(self->root);
            
        }
        self->root = NULL;
        self->count = 0;
        if(self->isLockSupported){
            sem_post(&self->lock);
        }
    }
    void LHeapRelease(struct LHeap* self){
        self->clear(self);
        if(self->isLockSupported){
        sem_destroy(&self->lock);
        }
    }
    struct LHeap* newLHeap(unsigned int concurrent_count,int (*fetchKeyFromValue)(struct LHeap*,void*),int (*comparator)(struct LHeap*,void*,void*)){
        struct LHeap* linked_heap = (struct LHeap*)malloc(sizeof(struct LHeap));
        memset(linked_heap,0,sizeof(struct LHeap));
        linked_heap->isLockSupported = (concurrent_count>0);
        if(linked_heap->isLockSupported){
            sem_init(&linked_heap->lock,0,concurrent_count);
        }
        
        linked_heap->fetchKeyFromValue = fetchKeyFromValue;
        linked_heap->comparator = comparator;
        linked_heap->push = LHeapPush;
        linked_heap->pop = LHeapPop;
        linked_heap->get = LHeapGet;
        linked_heap->release = LHeapRelease;
        linked_heap->isEmpty = LHeapIsEmpty;
        linked_heap->getCount = LHeapGetCount;
        linked_heap->clear = LHeapClear;
        return linked_heap;
    }
    struct LHeap* newLHeap2(unsigned int concurrent_count,int (*fetchKeyFromValue)(struct LHeap*,void*)){
        return newLHeap(concurrent_count,fetchKeyFromValue,defaultComparator);
    }