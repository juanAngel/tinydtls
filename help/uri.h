#ifndef URI_H_
#define URI_H_

//=================================================================================================

#include "../bajoNivel/tipos.h"
#include <tcpip/tcpip.h>		// IP_ADDRESS_TYPE

//=================================================================================================

// Representación de una URI
typedef struct Uri URI;

/**
 * Crea una URI vacía
 *
 * @return					La URI vacía
 */
URI *Uri_createEmpty(void);

/**
 * Establece el host de la URI
 *
 * @param uri				La URI
 * @param host				El host a establecer
 * @return					TRUE si se ha podido completar la operación o FALSE en caso contrario
 */
BYTE Uri_setHost(URI *uri, char *host);

/**
 * Establece el puerto de la URI
 *
 * @param uri				La URI
 * @param port				El puerto a establecer
 * @return					TRUE si se ha podido completar la operación o FALSE en caso contrario
 */
BYTE Uri_setPort(URI *uri, WORD port);

/**
 * Establece la ruta de la URI
 *
 * @param uri				La URI
 * @param port				La ruta a establecer
 * @return					TRUE si se ha podido completar la operación o FALSE en caso contrario
 */
BYTE Uri_setPath(URI *uri, char *path);

/**
 * Establece la consulta de la URI
 *
 * @param uri				La URI
 * @param port				La consulta a establecer
 * @return					TRUE si se ha podido completar la operación o FALSE en caso contrario
 */
BYTE Uri_setQuery(URI *uri, char *query);

/**
 * Parsea una URI a partir de una cadena que representa una URI
 *
 * @param str				La cadena a parsear
 * @param prefix			El prefijo (por ejemplo "coap://") (si es NULL se descarta sencillamente)
 * @param portToUseIfNotSet	El puerto a establecer si no se detecta ninguno al parsear
 * @param boardName			Es el DNS de la placa (NULL si no se quiere usar)
 * @param ipType			IPv4/IPv6 (en ambos casos se comprueba antes el "boardName" en caso de que se haya indicado) 
 * @return					La representación de la URI dividida o NULL si no se pudo realizar
 */
URI *Uri_parseFromString(char *str, const char *prefix, WORD portToUseIfNotSet, char *boardName, IP_ADDRESS_TYPE ipType);

/**
 * Obtiene el host de una URI
 *
 * @param uri				La URI
 * @return					El host o NULL si no lo hay
 */
char *Uri_getHost(URI *uri);

/**
 * Obtiene el puerto de una URI
 *
 * @param uri				La URI
 * @return					El puerto
 */
WORD Uri_getPort(URI *uri);

/**
 * Obtiene el path de una URI
 *
 * @param uri				La URI
 * @return					El path o NULL si no lo hay
 */
char *Uri_getPath(URI *uri);

/**
 * Obtiene el query de una URI
 *
 * @param uri				La URI
 * @return					El query o NULL si no lo hay
 */
char *Uri_getQuery(URI *uri);

/**
 * Libera la memoria de la URI
 *
 * @param uri				La URI a liberar
 */
void Uri_free(URI *uri);

//=================================================================================================

#endif
