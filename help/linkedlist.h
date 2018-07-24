#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include <stdbool.h>


typedef void (*linkedList_free_item)(void* item);
typedef void (*linkedList_foreach_functor)(void* data,void* item);
typedef int (*linkedList_comparator)(void* item1,void* item2);

typedef void linkedList_nodo;
typedef void linkedList;
typedef linkedList_nodo* linkedList_iterator;


linkedList* linkedList_new(linkedList_free_item free_item);
void linkedList_free(linkedList* list);
void linkedList_clear(linkedList* list);
linkedList_iterator linkedList_addItemFrom(linkedList* list,void* item);
linkedList_iterator linkedList_addItemBack(linkedList* list,void* item);
linkedList_iterator linkedList_removeItem(linkedList* list,linkedList_iterator it);

linkedList_iterator linkedList_begin(linkedList* list);
linkedList_iterator linkedList_end(linkedList* list);
linkedList_iterator linkedList_next(linkedList_iterator it);

void linkedList_foreach(linkedList* list,linkedList_foreach_functor f,void* data);
void* linkedList_getItem(linkedList_iterator it);
void linkedList_setItem(linkedList_iterator it);
linkedList_iterator linkedList_find(linkedList* list,void* item,linkedList_comparator criterio);

inline bool linkedList_empty(linkedList* list){
    return linkedList_begin(list) == linkedList_end(list);
}

#endif // LINKEDLIST_H
