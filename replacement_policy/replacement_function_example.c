#include "../shared/doubly_linked_list.h"
#include "../shared/memory_buffer.h"
//완벽하게 정해진 것은아니고 호출할 example 임
/** flush thread쪽 호출할때 해당 로직을 검토중
 * if(isBufferContainerFull==true){
 *      int replacement_algorithm_type = -1;
 *      struct DoublyLinkedList* evictable_item = pickEvictableItem(buffer_container,replacement_algorithm_type);
 *      if(evictable_item!=NULL){
 *          write2DiskIfDirty(evictable_item);
 *          resourceCleanUpBeforeEviction(buffer_container,evictable_item,replacement_algorithm_type);
 *          popFromDoublyLinkedList(evictable_item);
 *      }
 *      
 * }
 * 아래는 단지 예제이므로 이런 느낌이면 좋다라는 의미의 코드
 * 
*/
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
    if(container->current!=NULL){
        current_buffer = container->current;
        /**
         * 여러 캐시 알고리즘으로 탐색 구현
         * //current = found_buffer
        */
       return current_buffer;
    }
    return NULL;
}
int resourceCleanUpBeforeEviction(struct DoublyLinkedListWrapper* container,struct DoublyLinkedList* evictable_item,int replacement_algorithm_type){
    int  is_cleaned_resource = 1;//true
    /**
     * 내부 캐시알고리즘에서 리소스자체적으로 release하거나 축출할 리스트가 명시적으로 있을떄 사용
     * 라이프 사이클상 destroy 나 exit에 해당하는 함수
     * 
    */
    if(is_cleaned_resource==1){
        return 1;
    }
    return 0;//false
}