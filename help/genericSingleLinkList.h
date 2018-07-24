#ifndef GENERIC_SINGLE_LINK_LIST_H_
#define GENERIC_SINGLE_LINK_LIST_H_

#include "../bajoNivel/tipos.h"

//=================================================================================================

typedef struct GenericSingleLinkList  GENERIC_SINGLE_LINK_LIST;

//=================================================================================================

/**
 * Crea una nueva lista generica de simple enlace.
 *
 * @param freeItem			Puntero a la función para liberar la memoria de un item.
 * @param compareTo			Puntero a la función a utilizar para comparar dos items.
 * @return 					Puentero a la lista creada o NULL en caso de que no se pudiera crear.
 */
GENERIC_SINGLE_LINK_LIST *GenericSingleLinkList_new(void (*free_item)(void *), int (*compareTo)(void *a, void *b));

/**
 * Obtiene el número de elementos que hay en la lista.
 *
 * @param list				La lista.
 * @return					El número de elementos.
 */
WORD GenericSingleLinkList_getCount(GENERIC_SINGLE_LINK_LIST *list);

/**
 * Obtiene un item de una posición en concreto.
 *
 * @param list				La lista.
 * @param pos				La posición del item a obtener.
 * @return					El item o NULL si no se encontró.
 */
void *GenericSingleLinkList_getItemAtIndex(GENERIC_SINGLE_LINK_LIST *list, WORD pos);

/**
 * Obtiene el indice de un item.
 *
 * @param list				La lista.
 * @param item				El item que buscamos si existe.
 * @return					El indice de item si existe o 0xFFFF si no existe.
 */
WORD GenericSingleLinkList_indexOf(GENERIC_SINGLE_LINK_LIST *list, void *item);

/**
 * Inserta un item al final de la lista.
 *
 * @param list				La lista.
 * @param item				El item a insertar.
 * @return					TRUE si se pudo insertar o FALSE en caso contrario.
 */
BYTE GenericSingleLinkList_insertItem(GENERIC_SINGLE_LINK_LIST *list, void *item);

/**
 * Inserta un item en la posición especificada
 *
 * @param list				La lista.
 * @param item				El item a insertar.
 * @param pos				La posición de insercción.
 * @return					TRUE si se pudo insertar o FALSE en caso contrario.
 */
BYTE GenericSingleLinkList_insertItemAtIndex(GENERIC_SINGLE_LINK_LIST *list, void *item, WORD pos);

/**
 * Mueve un item de una posición dada al principio de la lista.
 *
 * @param list				La lista.
 * @param pos				La posición a mover.
 * @return					TRUE si se ha podido realizar la operación o FALSE en caso contrario.
 */
BYTE GenericSingleLinkList_moveItemFromIndexToFirst(GENERIC_SINGLE_LINK_LIST *list, WORD pos);

/**
 * Mueve un item de una posición dada al final de la lista.
 *
 * @param list				La lista.
 * @param pos				La posición a mover.
 * @return					TRUE si se ha podido realizar la operación o FALSE en caso contrario.
 */
BYTE GenericSingleLinkList_moveItemFromIndexToLast(GENERIC_SINGLE_LINK_LIST *list, WORD pos);

/**
 * Elimina un item de la lista.
 *
 * @param list				La lista.
 * @param item				El item a borrar.
 * @return					TRUE si se pudo borrar o FALSE en caso contrario.
 */
BYTE GenericSingleLinkList_removeItem(GENERIC_SINGLE_LINK_LIST *list, void *item);

/**
 * Elimina un item de la lista dada su posición.
 *
 * @param list				La lista.
 * @param pos				La posición del item a borrar.
 * @return					TRUE si se pudo borrar o FALSE en caso contrario.
 */
BYTE GenericSingleLinkList_removeItemAtIndex(GENERIC_SINGLE_LINK_LIST *list, WORD pos);

/**
 * Elimina todos los elementos de la lista, sin destruirla.
 *
 * @return 					TRUE si se pudo realizar la operación o FALSE en caso contrario.
 */
BYTE GenericSingleLinkList_clear(GENERIC_SINGLE_LINK_LIST *list);

/**
 * Elimina la lista.
 *
 * @return 					TRUE si pudo eliminar o FALSE en caso contrario.
 */
BYTE GenericSingleLinkList_free(GENERIC_SINGLE_LINK_LIST *list);

//=================================================================================================

#endif
