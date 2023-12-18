#include "replacement_policy.h"
//빌드 안되는 코드 주석처리
/*int initializeReplacementPolicyAlgorithms(void* args){
    static int is_init = 0;

    if(is_init == 0)
    {
        is_init = 1;
    
    }
}

int putOrCalcuateNewItem(struct DoublyLinkedList* currentItemContainer,void* args){
    int success = 0;

    return success;
}

void updateCacheStateDueToFetching(struct DoublyLinkedList* selected_item,void* args){
    struct MemoryBuffer* current_buffer =  (struct MemoryBuffer*) selected_item->value;
    /**
     * 각 캐시 알고리즘별로 접근 횟수 카운터등을 업데이트해주는 함수
    */
}
struct DoublyLinkedList* pickEvictableItem(struct DoublyLinkedListWrapper* container,int replacement_algorithm_type){// 이후 캐시나 arc알고리즘의 데이터 구조로 변경될 수 있음
    if(replacement_algorithm_type<0){
        //optimal알고리즘 말고 모든 알고리즘에서 최적화된 알고리즘으로 탐색고려
    }
    else if(replacement_algorithm_type==0){
        //lru 탐색
    }
    else{
        //기타등등
    }
    struct MemoryBuffer* current_buffer = NULL;
    struct DoublyLinkedList* current_position = NULL;
    if(container->current!=NULL){
        current_position = container->current;
        if(current_position->value!=NULL && sizeof(current_position->value)==sizeof(struct MemoryBuffer*)){
            current_buffer = (struct MemoryBuffer*) current_position->value;
        }

       return current_buffer;
    }
    return NULL;
}
int resourceCleanUpBeforeEvicting(struct DoublyLinkedListWrapper* container,struct DoublyLinkedList* evictable_item,int replacement_algorithm_type){
    int  is_cleaned_resource = 1;//true

    if(is_cleaned_resource==1){
        return 1;
    }
    return 0;//false
}*/