#include <stdlib.h>
#include <stdbool.h>

typedef void (*linkedList_free_item)(void* item);
typedef void (*linkedList_foreach_functor)(void* data,void* item);
typedef int (*linkedList_comparator)(void* item1,void* item2);
typedef struct linkedList_nodo{
    struct linkedList_nodo* next;
    void* item;
}linkedList_nodo;
typedef struct linkedList{
    linkedList_nodo* begin;
    linkedList_nodo** end;
    linkedList_free_item free_item;

}linkedList;
typedef linkedList_nodo** linkedList_iterator;

void linkedList_clear(linkedList *list);
linkedList_iterator linkedList_removeItem(linkedList *list, linkedList_iterator it);
linkedList_iterator linkedList_begin(linkedList *list);
linkedList_iterator linkedList_end(linkedList *list);

linkedList *linkedList_new(linkedList_free_item free_item){
    linkedList* list = (linkedList*)malloc(sizeof(*list));

    list->begin = NULL;
    list->end = &list->begin;
    list->free_item = free_item;

    return list;
}

void linkedList_free(linkedList *list){
    linkedList_clear(list);
    free(list);
}

void linkedList_clear(linkedList *list){
    linkedList_iterator it;
    //if(list && list->begin)
        for( it = linkedList_begin(list);
            it != linkedList_end(list);
            it = linkedList_removeItem(list,it));
}

linkedList_iterator linkedList_addItemFrom(linkedList* list,void* item){
    linkedList_nodo* tmp = (linkedList_nodo*)malloc(sizeof(*tmp));
    tmp->item = item;
    tmp->next = list->begin;

    if(list->begin == NULL){
        list->end = &tmp->next;
    }
    list->begin = tmp;


    return &list->begin;
}

linkedList_iterator linkedList_addItemBack(linkedList* list,void* item){

    linkedList_nodo* tmp = (linkedList_nodo*)malloc(sizeof(*tmp));
    linkedList_iterator it = list->end;
    tmp->item = item;
    tmp->next = NULL;
    (*it) = tmp;
    list->end = &(*it)->next;

    return it;
}

linkedList_iterator linkedList_removeItem(linkedList *list, linkedList_iterator it){
    linkedList_nodo* tmp = *it;
    if(it && tmp){
        if(&(*it)->next == list->end)
            list->end = it;
        (*it) = (*it)->next;

        if(list->free_item)
            list->free_item(tmp->item);
        free(tmp);
    }

    return it;
}

linkedList_iterator linkedList_begin(linkedList *list){
    return &list->begin;
}

linkedList_iterator linkedList_end(linkedList *list){
    return list->end;
}

linkedList_iterator linkedList_next(linkedList_iterator it){
    return &(*it)->next;
}

void *linkedList_getItem(linkedList_iterator it){
    return (*it)->item;
}
void linkedList_setItem(linkedList_iterator it,void* item){
    (*it)->item = item;
}

linkedList_iterator linkedList_find(linkedList *list, void *item, linkedList_comparator criterio){
    bool result = false;

    linkedList_iterator it,lastIt;
    for(it = linkedList_begin(list),lastIt = linkedList_end(list);
            !result && it != linkedList_end(list);
            it = linkedList_next(it)){
        lastIt = it;
        result = criterio(item,linkedList_getItem(it)) == 0;

    }

    return lastIt;
}

void linkedList_foreach(linkedList *list, linkedList_foreach_functor f, void *data){
    linkedList_iterator it;
    for(it = linkedList_begin(list);
            it != linkedList_end(list);
            it = linkedList_next(it)){
        f(data,linkedList_getItem(it));
    }
}
