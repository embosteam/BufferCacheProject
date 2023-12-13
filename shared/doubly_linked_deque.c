#include "doubly_linked_deque.h"

struct DoublyLinkedDequeInternalNode* newDoublyLInkedDequeInternalNode(void* value){
    struct DoublyLinkedDequeInternalNode* node = (struct DoublyLinkedDequeInternalNode*)malloc(sizeof(struct DoublyLinkedDequeInternalNode));
    memset(node,0,sizeof(struct DoublyLinkedDequeInternalNode));
    node->value = value;
    return node;
}

int popFirst(struct DoublyLinkedDeque* self){
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }
    int return_value = 0;
    if(self->count>1){
       struct DoublyLinkedDequeInternalNode* current_node =  self->first;
       if(current_node!=NULL){
            struct DoublyLinkedDequeInternalNode* next_node = current_node->next;
            self->first = next_node;
            next_node->previous = NULL;
            free(current_node);
            self->count--;
            return_value = 1;
       }
       else{
        if(self->isLockSupported){
            sem_post(&self->lock);
        }
        return -1;
       }
    }
    else if(self->count==1){
        struct DoublyLinkedDequeInternalNode* current_node =  self->first;
        free(current_node);
        self->first = NULL;
        self->last = NULL;
        self->count--;
        return_value = 1;
    }

    if(self->isLockSupported){
        sem_post(&self->lock);
    }
    return return_value;
}
int popLast(struct DoublyLinkedDeque* self){
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }
    int return_value = 0;
    if(self->count>1){
       struct DoublyLinkedDequeInternalNode* current_node =  self->last;
       if(current_node!=NULL){
            struct DoublyLinkedDequeInternalNode* next_node = current_node->previous;
            self->last = next_node;
            next_node->next = NULL;
            free(current_node);
            self->count--;
            return_value = 1;
       }
       else{
        if(self->isLockSupported){
            sem_post(&self->lock);
        }
        return -1;
        
       }

    }
    else if(self->count==1){
        struct DoublyLinkedDequeInternalNode* current_node =  self->last;
        free(current_node);
        self->first = NULL;
        self->last = NULL;
        self->count--;
        return_value = 1;
    }

    if(self->isLockSupported){
        sem_post(&self->lock);
    }
    return return_value;
}
int popAt(struct DoublyLinkedDeque* self,struct DoublyLinkedDequeInternalNode* at){
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }
    int return_value = 0;
    if(at!=NULL){
        const unsigned int count = self->count;
        
        struct DoublyLinkedDequeInternalNode* at_prev = at->previous;
        struct DoublyLinkedDequeInternalNode* at_next = at->next;
        unsigned char isAtFirstNode = at==self->first;
        unsigned char isAtLastNode = at==self->last;
        
        
        if(at_prev!=NULL){
            at_prev->next = at_next;
        }

        if(at_next!=NULL){
            at_next->previous = at_prev;
        }

        if(isAtLastNode){
            self->last = at_prev;
        }
        if(isAtFirstNode){
            self->first = at_next;
        }
        self->count--;
        return_value = 1;
    }
    
    if(self->isLockSupported){
        sem_post(&self->lock);
    }
    return return_value;
}
struct DoublyLinkedDequeInternalNode* pushFirst(struct DoublyLinkedDeque* self,void* value){
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }

    struct DoublyLinkedDequeInternalNode* new_node = newDoublyLInkedDequeInternalNode(value);


    if(self->count==0){
        new_node->value = value;
        self->first = new_node;
        self->last = new_node;
        
        
    }
    else{
        struct DoublyLinkedDequeInternalNode* current_node = self->first;
        current_node->previous = new_node;
        new_node->next = current_node;
        self->first = new_node;
    }
    self->count++;

    if(self->isLockSupported){
        sem_post(&self->lock);
    }
    return new_node;
}
struct DoublyLinkedDequeInternalNode* pushLast(struct DoublyLinkedDeque* self, void* value){
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }

    struct DoublyLinkedDequeInternalNode* new_node = newDoublyLInkedDequeInternalNode(value);

    

    if(self->count==0){
        new_node->value = value;
        self->first = new_node;
        self->last = new_node;
    }
    else{
        struct DoublyLinkedDequeInternalNode* current_node = self->last;
        current_node->next = new_node;
        new_node->previous = current_node;
        self->last = new_node;
    }
    self->count++;

    if(self->isLockSupported){
        sem_post(&self->lock);
    }
    return new_node;
}
int insertLeftAt(struct DoublyLinkedDeque* self,struct DoublyLinkedDequeInternalNode* at,void* value){
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }

    int return_result = 0;

    if(at!=NULL){
        struct DoublyLinkedDequeInternalNode* new_node = newDoublyLInkedDequeInternalNode(value);
        struct DoublyLinkedDequeInternalNode* at_prev = at->previous;
        new_node->next = at;
        new_node->previous = at_prev;
        at->previous = new_node;

        return_result = 1;
    }


    if(self->isLockSupported){
        sem_post(&self->lock);
    }
    return return_result;
}
int insertRightAt(struct DoublyLinkedDeque* self,struct DoublyLinkedDequeInternalNode* at,void* value){
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }

    int return_result = 0;

    if(at!=NULL){
        struct DoublyLinkedDequeInternalNode* new_node = newDoublyLInkedDequeInternalNode(value);
        struct DoublyLinkedDequeInternalNode* at_prev = at->next;
        new_node->previous = at;
        new_node->next = at_prev;
        at->next = new_node;

        return_result = 1;
    }

    if(self->isLockSupported){
        sem_post(&self->lock);
    }
    return return_result;
}
int switchNode(struct DoublyLinkedDeque* self,struct DoublyLinkedDequeInternalNode* dest,struct DoublyLinkedDequeInternalNode* src){
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }
    int return_result =0;
    if(dest!=NULL&&src!=NULL){
        struct DoublyLinkedDequeInternalNode* dest_prev = dest->previous;
        struct DoublyLinkedDequeInternalNode* dest_next = dest->next;

        struct DoublyLinkedDequeInternalNode* src_prev = src->previous;
        struct DoublyLinkedDequeInternalNode* src_next = src->next;
        
        dest_prev->next = src;
        dest_next->previous = src;
        src->previous = dest_prev;
        src->next = dest_next;

        src_prev->next = dest;
        src_next->previous = dest;
        dest->previous = src_prev;
        dest->next = src_next;
        return_result = 1;
    }
    if(self->isLockSupported){
        sem_post(&self->lock);
    }
    return return_result;
}
struct DoublyLinkedDequeInternalNode* getFirst(struct DoublyLinkedDeque* self){
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }

    struct DoublyLinkedDequeInternalNode* value = self->first;

    if(self->isLockSupported){
        sem_post(&self->lock);
    }
    return value;
}
struct DoublyLinkedDequeInternalNode* getLast(struct DoublyLinkedDeque* self){
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }

    struct DoublyLinkedDequeInternalNode* value = self->last;

    if(self->isLockSupported){
        sem_post(&self->lock);
    }
    return value;
}
void* getFirstValue(struct DoublyLinkedDeque* self){

    if(self->isLockSupported){
        sem_wait(&self->lock);
    }
    void* value = NULL;
    struct DoublyLinkedDequeInternalNode* node = self->first;
    if(node!=NULL){
        value = node->value;
    }
    if(self->isLockSupported){
        sem_post(&self->lock);
    }
    return value;
}
void* getLastValue(struct DoublyLinkedDeque* self){
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }

    void* value = NULL;
    struct DoublyLinkedDequeInternalNode* node = self->last;
    if(node!=NULL){
        value = node->value;
    }

    if(self->isLockSupported){
        sem_post(&self->lock);
    }
    return value;
}
unsigned int getCount(struct DoublyLinkedDeque* self){
    if(self->isLockSupported){
        sem_wait(&self->lock);
    }
    const unsigned int value = self->count;
    if(self->isLockSupported){
        sem_post(&self->lock);
    }
    return value;
}

struct DoublyLinkedDeque* newDoublyLinkedDeque(unsigned int concurrent_count){
        struct DoublyLinkedDeque* self = (struct DoublyLinkedDeque*)malloc(sizeof(struct DoublyLinkedDeque));
        memset(self,0,sizeof(struct DoublyLinkedDeque));
        self->isLockSupported = concurrent_count>0;
        if(self->isLockSupported){
            sem_init(&self->lock,0,concurrent_count);
        }
        self->popFirst = popFirst;
        self->popLast = popLast;
        self->popAt = popAt;
        self->pushFirst = pushFirst;
        self->pushLast = pushLast;
        self->insertLeftAt = insertLeftAt;
        self->insertRightAt = insertRightAt;
        self->switchNode = switchNode;
        self->getFirstValue = getFirstValue;
        self->getLastValue = getLastValue;
        self->getFirst = getFirst;
        self->getLast = getLast;
        self->getCount = getCount;
        
        return self;
}