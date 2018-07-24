#ifndef ARBOLNARIO_H_
#define ARBOLNARIO_H_

#include "../bajoNivel/tipos.h"

//=================================================================================================

// Tipo árbol n-ario
typedef struct Arbol_n_ario ARBOL_N_ARIO;

// Tipo posición de un nodo hoja
typedef struct Node POSICION;

//=================================================================================================

/**
 * Crear un arbol n-ario nuevo.
 *
 * @param free_item 		Puntero a función a utilizar para liberar la memoria de un item.
 *
 * @return					El árbol creado o NULL si no se pudo crear.
 */
ARBOL_N_ARIO *Arbol_n_ario_new(void (*free_item)(void *));

/**
 * Inserta un nodo como hijo del nodo al que apunta posición.
 *
 * @param tree				El árbol.
 * @param pos				La posición de inserción.
 * @param item				El elemento a insertar.
 *
 * @return					La posición del nodo insertado o NULL si error.
 */
POSICION *Arbol_n_ario_insertAtPosition(ARBOL_N_ARIO *tree, POSICION *pos, void *item);

/**
 * Borra un nodo del árbol.
 *
 * @param tree				El árbol.
 * @param pos				La posición del nodo a eliminar.
 *
 * @return					TRUE si se pudo borrar o FALSE en caso contrario.
 */
BYTE Arbol_n_ario_deleteNode(ARBOL_N_ARIO *tree, POSICION *pos);

/**
 * Obtiene la raíz del árbol.
 *
 * @param tree				El árbol.
 *
 * @return					La posición del nodo raíz o NULL si el árbol está vacío.
 */
POSICION *Arbol_n_ario_getRoot(ARBOL_N_ARIO *tree);

/**
 * Obtiene el primer hijo de un nodo dada su posición.
 *
 * @param pos				La posición del nodo del que deseamos obtener su primer hijo.
 *
 * @return					La posición del primer hijo o NULL si no tiene.
 */
POSICION *Arbol_n_ario_getFirstChild(POSICION *pos);

/**
 * Obtiene el siguiente hermano de un nodo dada su posición.
 *
 * @param pos				La posición del nodo del que deseamos obtener su siguiente hermano.
 *
 * @return					La posición del siguiente hermano o NULL si no tiene.
 */
POSICION *Arbol_n_ario_getNextBrother(POSICION *pos);

/**
 * Obtiene el padre de un nodo dada su posición.
 *
 * @param pos				La posición del nodo del que deseamos conocer su padre.
 *
 * @return					La posición del padre del nodo o NULL si no tiene.
 */
POSICION *Arbol_n_ario_getFather(POSICION *pos);

/**
 * Obtiene el item de un nodo dada su posición.
 *
 * @param pos				La posición del nodo del que deseamos extraer el item.
 *
 * @return					Puntero al item o NULL si no existe.
 */
void *Arbol_n_ario_getItem(POSICION *pos);

/**
 * Devuelve el número de elementos almacenados en el árbol.
 *
 * @param tree				El árbol.
 *
 * return					El número de elementos del arbol.
 */
WORD Arbol_n_ario_getCount(ARBOL_N_ARIO *tree);

/**
 * Obtiene el número de hijos que tiene un nodo del árbol.
 *
 * @param pos				La posición del nodo.
 *
 * @return					El número de hijos del nodo.
 */
WORD Arbol_n_ario_getChildCount(POSICION *pos);

/**
 * Devuelve el hijo i-esicmo de un nodo.
 *
 * @param pos				La posición del nodo.
 * @param index				El indice del hijo de 0 a Arbol_n_ario_getChildCount() - 1.
 * @return					Devuelve el hijo i-esimo o NULL si no existe.
 */
POSICION *Arbol_n_ario_getChildAtIndex(POSICION *pos, WORD index);

/**
 * Elimina el árbol completamente.
 *
 * @param tree				El árbol.
 */
void Arbol_n_ario_free(ARBOL_N_ARIO *tree);

//=================================================================================================

#endif
