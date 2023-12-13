#pragma once
#ifndef __DOUBLY_LINKED_DEQUE__
#define __DOUBLY_LINKED_DEQUE__
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <memory.h>
    #include <semaphore.h>
    struct DoublyLinkedDequeInternalNode{
        struct DoublyLinkedList* previous;
        struct DoublyLinkedList* next;
        void* value;
    };
    struct DoublyLinkedDeque{
        /**
            isLockSupported,lock은 concurrent 를 위함
        */
        unsigned char isLockSupported:1;
        sem_t lock;
        unsigned int count;//내부 노드 갯수를 저장하는 변수(concurrent보장하려면 getCount함수를 사용해 가져오길 바람)
        struct DoublyLinkedDequeInternalNode* first;//deque의 앞쪽 포인터
        struct DoublyLinkedDequeInternalNode* last;//deque의 뒤쪽 포인터
        int (*popFirst)(struct DoublyLinkedDeque* self);//앞쪽 포인터 값을 제거함
        int (*popLast)(struct DoublyLinkedDeque* self);//뒤쪽 포인터 값을 제거함
        int (*popAt)(struct DoublyLinkedDeque* self,struct DoublyLinkedDequeInternalNode* at);//특정 위치의 포인터 값을 제거함
        struct DoublyLinkedDequeInternalNode* (*pushFirst)(struct DoublyLinkedDeque* self,void* value);//deque앞쪽에 값을 집어넣음
        struct DoublyLinkedDequeInternalNode* (*pushLast)(struct DoublyLinkedDeque* self, void* value);//deque뒤쪽에 값을 집어넣음
        int (*insertLeftAt)(struct DoublyLinkedDeque* self,struct DoublyLinkedDequeInternalNode* at,void* value);//at변수 왼쪽에 값을 집어넣음
        int (*insertRightAt)(struct DoublyLinkedDeque* self,struct DoublyLinkedDequeInternalNode* at,void* value);//at변수 오른쪽에 값을 집어넣음
        int (*switchNode)(struct DoublyLinkedDeque* self,struct DoublyLinkedDequeInternalNode* dest,struct DoublyLinkedDequeInternalNode* src);//dest와 src의 위치를 바꿈
        void* (*getFirstValue)(struct DoublyLinkedDeque* self);//앞쪽 값을 가져옴
        void* (*getLastValue)(struct DoublyLinkedDeque* self);//뒤쪽 값을 가져옴
        unsigned int (*getCount)(struct DoublyLinkedDeque* self);//노드 갯수를 출력
        struct DoublyLinkedDequeInternalNode* (*getFirst)(struct DoublyLinkedDeque* self);//raw한 앞쪽 포인터를 가져옴
        struct DoublyLinkedDequeInternalNode* (*getLast)(struct DoublyLinkedDeque* self);//raw한 앞쪽 포인터를 가져옴
        
    };
    /**
        doubly linked deque를 생성하는 함수 사실상 new DoublyLinkedDeque(concurrent_count);
        concurrent_count: 세마포어 내부 갯수
    */
    struct DoublyLinkedDeque* newDoublyLinkedDeque(unsigned int concurrent_count);
#endif