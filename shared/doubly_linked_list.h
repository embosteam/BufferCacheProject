#ifndef __DOUBLY_LINKED_LIST__
    #define __DOUBLY_LINKED_LIST__
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <memory.h>

    /**
     * 더블 링크드 리스트 구조체
     * value: c에서 collection지원 안해서 struct Wrapper 등을 만들어 캐스트해서 사용바람
    */
    struct DoublyLinkedList{
        struct DoublyLinkedList* previous;
        struct DoublyLinkedList* next;
        void* value;
    };
    /**
     * doubly linked list를 전반적으로 객체단위로 관리할 수 있는 구조체
     *      list_size: 하부에 연결되어 있는 링크드 리스트 노드의 갯수
     *      root: base가 되는 doubly linked list 노드 객체
     *      current: 현재 검색위치에 있는 doubly linked list 노드 객체
     * 
    */
    struct DoublyLinkedListWrapper{
        unsigned long long list_size;
        struct DoublyLinkedList* root;
        struct DoublyLinkedList* current;
    };
    /**
     * 링크드 리스트에서 연결될 수 있는 리스트 노드를 생성, 내부 값들은 0으로 초기화
    */
    struct DoublyLinkedList* createNewDoublyLinkedList();
    /**
     * 해당 pointer객체를 기준으로 next에 value가 포함된 새 링크드 리스트 노드를 추가하는 함수
     * 리턴값: pointer 매개변수의 다음 포인터를 반환
    */
    struct DoublyLinkedList* appendDoublyLinkedListAtNext(struct DoublyLinkedList* pointer,void* value);
    /**
     * 해당 pointer객체를 기준으로 previous에 value가 포함된 새 링크드 리스트 노드를 추가하는 함수
     * 리턴값: pointer 매개변수의 이전 포인터를 반환
    */
    struct DoublyLinkedList* appendDoublyLinkedListAtPrevious(struct DoublyLinkedList* pointer,void* value);
    /**
     * pointer 객체의 리소스를 해제하고 이전 포인터와 다음 포인터를 연결시켜주는 함수
     * 리턴값: 성공시 1, 실패시 0 값을 반환
    */
    int popFromDoublyLinkedList(struct DoublyLinkedList* pointer);


#endif