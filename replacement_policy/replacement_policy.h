#pragma once

#include ""

int initializeReplacementPolicyAlgorithms(void* args);
int putOrCalcuateNewItem(struct DoublyLinkedList* currentItemContainer,void* args);
void updateCacheStateDueToFetching(struct DoublyLinkedList* selected_item,void* args);

struct DoublyLinkedList* pickEvictableItem(struct DoublyLinkedListWrapper* container,int replacement_algorithm_type);
int resourceCleanUpBeforeEvicting(struct DoublyLinkedListWrapper* container,struct DoublyLinkedList* evictable_item,int replacement_algorithm_type);
