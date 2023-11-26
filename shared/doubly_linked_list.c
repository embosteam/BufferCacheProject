#include "doubly_linked_list.h"
    /**
     * 링크드 리스트에서 연결될 수 있는 리스트 노드를 생성, 내부 값들은 0으로 초기화
    */
    struct DoublyLinkedList* createNewDoublyLinkedList(){
        struct DoublyLinkedList* root = NULL;
        root = (struct DoublyLinkedList*)malloc(sizeof(struct DoublyLinkedList));
        memset(root,0,sizeof(struct DoublyLinkedList));
        return root;
    }
    /**
     * 해당 pointer객체를 기준으로 next에 value가 포함된 새 링크드 리스트 노드를 추가하는 함수
     * 리턴값: pointer 매개변수의 다음 포인터를 반환
    */
    struct DoublyLinkedList* appendDoublyLinkedListAtNext(struct DoublyLinkedList* pointer,void* value){
        struct DoublyLinkedList* next = NULL;
        if(pointer!=NULL){
            next = createNewDoublyLinkedList();
            next->value = value;
            if(pointer->next==NULL){
                pointer->next = next;
                next->previous = pointer;
            }
            else{
                struct DoublyLinkedList* next_pointer = pointer->next;
                pointer->next = next;
                next->previous = pointer;
                next->next = next_pointer;
                next_pointer->previous = next;
            }
        }
        return next;
    }
    /**
     * 해당 pointer객체를 기준으로 previous에 value가 포함된 새 링크드 리스트 노드를 추가하는 함수
     * 리턴값: pointer 매개변수의 이전 포인터를 반환
    */
    struct DoublyLinkedList* appendDoublyLinkedListAtPrevious(struct DoublyLinkedList* pointer,void* value){
        struct DoublyLinkedList* previous = NULL;
        if(pointer!=NULL){
            previous = createNewDoublyLinkedList();
            previous->value = value;
            if(pointer->next==NULL){
                pointer->previous = previous;
                previous->next = pointer;
            }
            else{
                struct DoublyLinkedList* previous_pointer = pointer->previous;
                pointer->previous = previous;
                previous->next = pointer;
                previous->previous = previous_pointer;
                previous_pointer->next = previous;
            }
        }
        return previous;
    }
    /**
     * pointer 객체의 리소스를 해제하고 이전 포인터와 다음 포인터를 연결시켜주는 함수
     * 리턴값: 성공시 1, 실패시 0 값을 반환
    */
    int popFromDoublyLinkedList(struct DoublyLinkedList* pointer){
        if(pointer!=NULL){
            struct DoublyLinkedList* previous = pointer->previous;
            struct DoublyLinkedList* next = pointer->next;
            previous->next = next;
            next->previous = previous;
            free(pointer);
            
            return 1;
        }
        return 0;
    }