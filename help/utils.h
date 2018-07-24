#ifndef UTILS_H_
#define UTILS_H_

#include "../bajoNivel/tipos.h"

//=================================================================================================

/* Se utiliza para calcular el número de dias transcurridos desde 1970
 * para ello nos desplazamos al calculo al 01/01/2001, para así poder obtener de manera
 * más sencilla el número de años transcurridos que fueron bisiestos ((año - 2000) - 1) / 4)
 */
#define DIAS_HASTA_FIN_2000				11323

// segundos transcurridos desde el 01/01/1970 hasta las 00:00:00 del 01/01/2000
#define SEGUNDOS_ENTRE_1970_Y_2000		946684800ULL

// Devuelve el BYTE alto de un WORD
#define Utils_hiByte(x)					((BYTE)(x >> 8))

// Devuelve el BYTE bajo de un WORD
#define Utils_loByte(x)					((BYTE)x)

//=================================================================================================

/**
 * Obtiene una cadena delimitada por un carácter determinado a partir de otra
 *
 * @param s					La cadena de entrada
 * @param index				Indice a partir del cual se comenzar a procesar
 * @param separator			Caracter separador
 * @param token				La subcadena de salida
 * @param trimSeparator		Indica si deseamos que se eliminen los caracteres separadores que precedan a un token.
 */
void Utils_strGetToken(BYTE *src, WORD *index, char separator, BYTE *token, BYTE trimSeparator);

/**
 * Marca un token de una cadena de entrada para procesarlo después con sprintf.
 * La idea es poner un NULL en la posición del separador. En caso de llegar al final
 * no hace falta poner el NULL (ya está), pero la diferencia en que en ese caso
 * se devuelve NULL (no hay más tokens pendientes) mientras que en general devuelve
 * el puntero a la siguiente posición "procesable"
 * 
 * @param src				La cadena de entrada
 * @param separator			El separador
 * @return					El puntero al inicio del siguiente token (NULL si no hay más)
 */
BYTE *Utils_markToken(BYTE *src, char separator);

/**
 * Busca el offset en el que está un carácter (separador) concreto a partir de cierta posición
 * en un búffer, teniendo en cuenta un número de caracteres máximo en el array (por si no hay un NULL)
 * 
 * @param buffer			El búffer
 * @param separador			El carácter a buscar
 * @param offsetInicial		El offset inicial
 * @param longitudBuffer	La longitud total del búffer
 * @return					El offset (0xFFFF si no se encuentra)
 */
WORD Utils_offsetNextSeparator(BYTE *buffer, BYTE separador, WORD offsetInicial, WORD longitudBuffer);

/**
 * Comprueba si el array de bytes que se le pasa empieza por el patrón indicado
 * 
 * @param ptr				El puntero a comprobar (ASCII-Z)
 * @param pattern			El patrón (ASCII-Z)
 * @return					¿Empieza por ese patrón?
 */
BYTE Utils_startsWith(BYTE *ptr, const char *pattern);

/**
 * Comprueba si el array de bytes que se le pasa acaba por el patrón indicado
 * 
 * @param ptr				El puntero a comprobar (ASCII-Z)
 * @param pattern			El patrón (ASCII-Z)
 * @return					¿Acaba por ese patrón?
 */
BYTE Utils_endsWith(BYTE *ptr, const char *pattern);

/**
 * Desplaza un conjunto de BYTEs dentro de un array hacia la izquierda
 * 
 * @param buffer			El búffer
 * @param desplazamiento	El número de posiciones a desplazar hacia la izquierda
 * @param offsetInicial		El offset del primer carácter a desplazar
 * @param numBytes			El número de caracteres a procesar
 */
void Utils_shiftLeft(BYTE *buffer, WORD desplazamiento, WORD offsetInicial, WORD numBytes);

/**
 * Crea un WORD a partir de 2 bytes
 *
 * @param hi                El byte superior
 * @param lo                El byte inferior
 * @return                  El WORD
 */
WORD Utils_joinBytes(BYTE hi, BYTE lo);

#ifndef Utils_copyBytes
/**
 * Copia un array de bytes de un sitio a otro
 * 
 * @param dest              El destino
 * @param src               El origen
 * @param len               El número de bytes
 */
void Utils_copyBytes(BYTE *dest, BYTE *src, WORD len);

#endif
/**
 * Muestra por "pantalla" los bytes indicados y el mensaje que se le pasa
 *
 * @param mensaje           El mensaje a mostrar
 * @param bytes             El puntero a la zona donde están los bytes a mostrar
 * @param len               El número de bytes a mostrar
 */
void Utils_printBytes(const char *mensaje, BYTE *bytes, WORD len);

/**
 * Muestra los datos como chars o bytes-hex, en función de si son imprimibles o no
 * (los no imprimibles se muestran con 2 caracteres, con un espacio en blanco antes
 * y otro después, y los imprimibles se muestran tal cual)
 * 
 * @param mensaje			El mensaje inicial
 * @param bytes				El puntero de los datos a mostrar
 * @param len				El número de bytes a mostrar
 */
void Utils_printBytesCombinadosConChars(const char *mensaje, BYTE *bytes, WORD len);

/**
 * Convierte los segundos que se le pasan a fecha (con los campos habituales)
 *
 * @param totalSeconds      Segundos totales actuales
 * @param segundos          Segundos del minuto actual (de 0 a 59)
 * @param minutos           Minutos de la hora actual (de 0 a 59)
 * @param horas             Hora dentro del día actual (de 0 a 23)
 * @param dia               Día actual (de 1 a 31)
 * @param mes               Mes actual (de 0 a 11)
 * @param anio              Año actual (de 0 a 99)
 * @param diaSemana         Día de la semana actual (lunes = 1, martes = 2, ..., domingo = 7)
 */
void Utils_convertSecondsToDate(DWORD totalSeconds, BYTE *segundos, BYTE *minutos, BYTE *horas, BYTE *dia,
      BYTE *mes, BYTE *anio, BYTE *diaSemana);

/**
 * Convierte una fecha a segundos
 *
 * @param segundos			Segundos del minuto actual (de 0 a 59)
 * @param minutos			Minutos de la hora actual (de 0 a 59)
 * @param horas				Horas del día actual (de 0 a 23)
 * @param dia				Día actual (de 1 a 31)
 * @param mes				Mes actual (de 0 a 11)
 * @param anio				El año actual (de 0 a 99)
 * @return					La fecha actual convertida a segundos
 */
DWORD Utils_convertDateToSeconds(BYTE segundos, BYTE minutos, BYTE horas, BYTE dia, BYTE mes, BYTE anio);

/**
 * Indica si estamos en horario de verano
 * 
 * @param currentTime		Los segundos actuales en el TimeZone actual desde el 01/01/2000 a las 00:00:00
 * @return					¿Estamos en horario de verano?
 */
BYTE Utils_estamosEnHorarioDeVerano(DWORD currentTime);

/**
 * Devuelve el día de la semana para un DD/MM/YYYY concreto
 * 
 * @param dia				El día (de 1 a 31)
 * @param mes				El mes (de 0 a 11)
 * @param anio				El año (valor absoluto, nada de "desde el 2000")
 * @return					El día de la semana
 */
BYTE Utils_getDayOfWeek(BYTE dia, BYTE mes, WORD anio);

/**
 * Muestra por la depuración la hora actual
 */
void Utils_debugTime(void);

/**
 * Hace el swap (intercambio) de los valores de un WORD
 * 
 * @param v                 El valor original
 * @return                  El valor tras el swapping
 */
WORD Utils_swapw(WORD v);

/**
 * Hace el swap (intercambio) de los valores de un DWORD
 * 
 * @param v                 El valor original
 * @return                  El valor tras el swapping
 */
DWORD Utils_swapd(DWORD v);

/**
 * Hace el swap (intercambio) de los valores de un int32
 * 
 * @param v                 El valor original
 * @return                  El valor tras el swapping
 */
int32 Utils_swapInt32(int32 v);

/**
 * Hace el swap (intercambio) de los valores de un QWORD
 * 
 * @param v					El valor inicial
 * @return					El valor final
 */
QWORD Utils_swapq(QWORD v);

/**
 * Hace el swap (intercambio) de los valores de un double64
 * 
 * @param d                 El valor original
 * @return                  El valor tras el swapping
 */
double64 Utils_swapDouble64(double64 d);

/**
 * Compara dos cadenas sin tener en cuenta mayúsculas y minúsculas
 * 
 * @param s1                La cadena de entrada 1
 * @param s2                La cadena de entrada 2
 * @return                  Resultado de la comparación de las dos cadenas. Si s1 > s2 devolverá un valor > 0, si
 *                          s1 < s2 devolverá un valor < 0 y en caso de que sean iguales devolverá 0
 */
int Utils_strcmpi(const char *s1, BYTE *s2);

/**
 * Rellena tantas posiciones como se indiquen con el valor que se le pasa
 * 
 * @param dest              La posiciún a partir de la que rellenamos
 * @param valor             El valor a introducir
 * @param len               El número de posiciones
 */
void Utils_fill(BYTE *dest,BYTE valor,DWORD len);

/**
 * Convierte una fecha (superior al año 2000) al número de días transcurridos desde 01/01/1970 (o desde el 01/01/2000)
 * 
 * @param dia               Día actual (de 1 a 31)
 * @param mes               Mes actual (de 0 a 11)
 * @param anio              El año actual (valor mayor a 2000)
 * @param isFrom1970		Indica si es desde 1970 o desde 2000
 * @return                  La fecha actual convertida a días
 */
DWORD Utils_convertDateToNumDays(BYTE dia, BYTE mes, WORD anio, BYTE isFrom1970);

/**
 * Devuelve el día de la semana en base a los datos indicados
 * 
 * @param dia				El día del mes (de 1 a 31)
 * @param mes				El mes (de 0 a 11)
 * @param anio				El año (posterior al 2000)
 * @return					El día de la semana (1 = lunes, 2 = martes, ..)
 */
BYTE Utils_getDayOfWeek(BYTE dia, BYTE mes, WORD anio);

/**
 * Extrae un byte de los 2 primeros dígitos del puntero recibido, asumiendo que van en ASCII-HEX
 * 
 * @param data              El origen
 * @return                  El BYTE (0xFF si alguno de los datos no es correcto)
 */
BYTE Utils_extractHexByte(BYTE *data);

/**
 * Convierte un carácter a mayúsculas
 * 
 * @param c                 El carácter a convertir
 * @return                  El carácter convertido
 */
char Utils_toUpper(BYTE c);

/**
 * Convierte un carácter a minúsculas
 * 
 * @param c                 El carácter a convertir
 * @return                  El carácter convertido
 */
char Utils_toLower(BYTE c);

/**
 * Convierte una cadena con digitos hexadecimales a entero
 *
 * @param hex               La cadena a convertir a entero
 * @return                  El nï¿½mero resultado de la cadena convertida
 */
DWORD Utils_asciiHexToInt(char *hex);

/**
 * Comprueba si el dato (realemnte es un char) es un dígito válido asumiendo que nos interesa
 * extraer un ASCII-HEX con él
 * 
 * @param dato              El dato
 * @return                  ¿Es compatible?
 */
BYTE Utils_isValidForAsciiHex(BYTE dato);

/**
 * Pasa de ASCII-HEX a BYTE (asumiendo que el dato es compatible)
 * 
 * @param dato              El dato en ASCII-HEX
 * @return                  El dato en BYTE
 */
BYTE Utils_convertAsciiHexToByte(BYTE dato);

/**
 * Escribe un WORD a su versión ASCII hexadecimal
 * 
 * @param data				El dato a escribir
 * @param dest				El destino donde escribir
 */
void Utils_writeWordToAsciiHex(WORD data, BYTE *dest);

/**
 * Convierte los 4 bits inferiores de un BYTE as la versión ASCII-HEX
 * 
 * @param bits				Los bits
 * @return					El carácter ASCII
 */
BYTE Utils_convert4lowerBitsToAsciiHex(BYTE bits);

/**
 * Intenta extraer un valor entero no negativo partiendo de una
 * cadena de caracteres, con un número de bytes concretos (lo limitamos
 * a 10, aunque en un DWORD caben más, claro)
 * 
 * @param data				La cadena de caracteres
 * @param lengthBytes		El número de caracteres a procesar SIEMPRE
 * @return					El valor (0xFFFFFFFF si no se puede extraer el entero)
 */
DWORD Utils_extractWordCharValue(BYTE *data, BYTE lengthBytes);

/**
 * Compara dos arrrays de bytes para ver si son iguales.
 * 
 * @param a                 Primer array.
 * @param b                 Segundo array.
 * @param len               Longitud a comparar.
 * @return                  TRUE si son iguales o FALSE en caso contrario.
 */
BYTE Utils_equal(BYTE *a, BYTE *b, WORD len);

/**
 * Calcula el CRC16-ITT de una trama de datos
 *
 * @param datos            Puntero con los datos a calcular el CRC
 * @param len              Tamaño de los datos en bytes
 * @return                 El checksum
 */
WORD Utils_calcCRC16ITT(BYTE *datos, DWORD len);

/**
 * Calcula el CRC16-ITT de una trama de datos que están en Dataflash
 *
 * @param offset			El offset inicial de los datos en Dataflash
 * @param len              Tamaño de los datos en bytes
 * @return                 El checksum.
 */
WORD Utils_calcCRC16ITTfromDataflash(DWORD offset, DWORD len);

/**
 * Calcula el CRC16-ITT de una trama de datos usando como base el CRC que se indique, lo que nos
 * permite ir calculando el CRC global "por partes" (en la primera llamada se debe pasar el
 * valor 0xFFFF)
 * 
 * @param crc				El CRC parcial
 * @param datos				Puntero con los datos a calcular el CRC
 * @param len				Tamaño de los datos en bytes
 * @return					El checksum
 */
WORD Utils_calcCRC16ITTparcial(WORD crc, BYTE *datos, DWORD len);

/**
 * Ordena un array de bytes de menor a mayor por el método de la burbuja
 *
 * @param data              Los datos a ordenar
 * @param len               El tamaño de los datos
 */
void Utils_sort(BYTE *data, WORD len);

/**
 * Pasa la placa a sleep
 */
void Utils_goToSleep(void);

/**
 * Pasa la placa a sleep activando el watchdog
 */
void Utils_goToSleepWithWatchdog(void);

/**
 * Extrae un WORD que viene de un array de bytes en BIG_ENDIAN
 * 
 * @param src				El puntero del que extraer el dato
 * @return					El dato
 */
WORD Utils_extractBigEndianWord(BYTE *src);

/**
 * Extrae un WORD que viene de un array de bytes en LITTLE_ENDIAN
 *
 * @param src				El puntero del que extraer el dato
 * @return					El dato
 */
WORD Utils_extractLittleEndianWord(BYTE *src);

/**
 * Extrae un int32 que viene de un array de bytes en BIG_ENDIAN
 * 
 * @param src				El puntero del que extraer el dato
 * @return					El dato
 */
int32 Utils_extractBigEndianInt32(BYTE *src);

/**
 * Extrae un int32 que viene de un array de bytes en LITTLE_ENDIAN
 * 
 * @param src				El puntero del que extraer el dato
 * @return					El dato
 */
int32 Utils_extractLittleEndianInt32(BYTE *src);

/**
 * Extrae un float que viene de un array de bytes en BIG_ENDIAN
 * 
 * @param src				El puntero del que extraer el dato
 * @return					El dato
 */
float Utils_extractBigEndianFloat(BYTE *src);

/**
 * Extrae un float que viene de un array de bytes en LITTLE_ENDIAN
 * 
 * @param src				El puntero del que extraer el dato
 * @return					El dato
 */
float Utils_extractLittleEndianFloat(BYTE *src);

/**
 * Extrae un QWORD que viene de un array de bytes en BIG_ENDIAN
 * 
 * @param src				El puntero del que extraer el dato
 * @return					El dato
 */
QWORD Utils_extractBigEndianQword(BYTE *src);

/**
 * Extrae un QWORD que viene de un array de bytes en LITTLE_ENDIAN
 * 
 * @param src				El puntero del que extraer el dato
 * @return					El dato
 */
QWORD Utils_extractLittleEndianQword(BYTE *src);

/**
 * Extrae un double64 que viene de un array de bytes en BIG_ENDIAN
 * 
 * @param src				El puntero del que extraer el dato
 * @return					El dato
 */
double64 Utils_extractBigEndianDouble64(BYTE *src);

/**
 * Extrae un double64 que viene de un array de bytes en LITTLE_ENDIAN
 * 
 * @param src				El puntero del que extraer el dato
 * @return					El dato
 */
double64 Utils_extractLittleEndianDouble64(BYTE *src);

/**
 * Escribe un WORD en un array de bytes en BIG_ENDIAN
 * 
 * @param valor				El valor
 * @param dest				El array destino
 */
void Utils_writeBigEndianWord(WORD valor, BYTE *dest);

/**
 * Escribe un WORD en un array de bytes en LITTLE_ENDIAN
 * 
 * @param valor				El valor
 * @param dest				El array destino
 */
void Utils_writeLittleEndianWord(WORD valor, BYTE *dest);

/**
 * Escribe un int32 en un array de bytes en BIG_ENDIAN
 * 
 * @param valor				El valor
 * @param dest				El array destino
 */
void Utils_writeBigEndianInt32(int32 valor, BYTE *dest);

/**
 * Escribe un QWORD en un array de bytes en LITTLE_ENDIAN
 * 
 * @param valor				El valor
 * @param dest				El array destino
 */
void Utils_writeLittleEndianQword(QWORD valor, BYTE *dest);

/**
 * Escribe un QWORD en un array de bytes en BIG_ENDIAN
 * 
 * @param valor				El valor
 * @param dest				El array destino
 */
void Utils_writeBigEndianQword(QWORD valor, BYTE *dest);

/**
 * Obtiene la dirección de grupo EIB a partir de sus componentes
 * 
 * @param area				La parte del área (4 bits)
 * @param linea				La parte de la línea (3 bits)
 * @param dispositivo		La parte del dispositivo (8 bits)
 * @return					La dirección
 */
WORD Utils_getDireccionGrupoEIB(BYTE area, BYTE linea, BYTE dispositivo);

/**
 * Convierte los datos en formato EIS type 5 (signo, exponente, mantisa) en punto flotante
 * 
 * @param data				El valor en formato signo+exponente+mantisa
 * @return					El valor en punto flotante
 */
double64 Utils_convertEisType5toFloatValue(WORD data);

/**
 * Convierte un valor en punto flotante en un valor en formato EIS type 5 (signo, exponente, mantisa)
 * 
 * @param data				El valor en punto flotante
 * @return					El valor en formato EIS type 5
 */
WORD Utils_convertFloatValueToEisType5(double64 floatValue);

/**
 * Convierte una cadena con dígitos decimales a entero
 *
 * @param txt				La cadena a convertir a entero
 * @param len				La longitud de la cadena a convertir
 * @return					El número resultado de la cadena convertida
 */
DWORD Utils_asciiToInt(BYTE *txt, BYTE len);

/**
 * Convierte una cadena con dígitos decimales a entero
 *
 * @param txt				La cadena a convertir a entero
 * @param len				La longitud de la cadena a convertir
 * @return					El número resultado de la cadena convertida
 */
DWORD Utils_asciiToInt(BYTE *txt, BYTE len);

/**
 * Desplaza bytes hacia la izquierda dentro de un array
 * 
 * @param data							El array de bytes
 * @param offset						El offset a partir del que empezamos a desplazar
 * @param numBytes						El número de bytes a desplazar
 * @param numPosicionesDesplazamiento	El número de posiciones hacia la izquierda que se desplazan
 */
void Utils_desplazarBytesHaciaIzquierda(BYTE *data, WORD offset, WORD numBytes, WORD numPosicionesDesplazamiento);

/**
 * Calcula el byte de paridad conforme al protocolo SIA de las centralitas antirobo Galaxy
 * 
 * @param data				Los datos
 * @param length			La longitud
 * @return					El byte de paridad (pseudo-CRC)
 */
BYTE Utils_getParityFromGalaxySIA(BYTE *data, BYTE length);

/**
 * Muestra los bytes indicados, junto al mensaje que se le especifica en modo mezcla
 * 
 * @param mensaje		El mensaje a mostrar
 * @param bytes			El puntero a la zona donde están los bytes a mostrar
 * @param len			El número de bytes a mostrar
 */
void Utils_printBytesMix(const char *mensaje, BYTE *bytes, WORD len);

/**
 * Indica si el carácter que recibe va de '0' a '9'
 * 
 * @param data				El carácter a comprobar
 * @return					¿Es un dígito ASCII?
 */
BYTE Utils_isAsciiDigit(char data);

/**
 * Calcula el bit de paridad para un BYTE, asumiendo que nos llegan 7 bits válidos (los inferiores)
 * y queremos establecer como bit superior el de paridad par
 * 
 * @param dato				El BYTE al que calcularle la paridad
 * @return					El BYTE con la paridad (si hay que ponerle un 1 se devuelve 0x80 y en otro caso se devuelve 0)
 */
BYTE Utils_calcularBitParidadPar(BYTE dato);

/**
 * Calcula el bit de paridad para un BYTE, asumiendo que nos llegan 7 bits válidos (los inferiores)
 * y queremos establecer como bit superior el de paridad impar
 * 
 * @param dato				El BYTE al que calcularle la paridad
 * @return					El BYTE con la paridad (si hay que ponerle un 1 se devuelve 0x80 y en otro caso se devuelve 0)
 */
BYTE Utils_calcularBitParidadImpar(BYTE dato);

///**
// * Se pasa N segundos encendiendo y apagando el led durante 250 milis (igual que se hace en el main)
// * para comprobar que la placa funciona bien
// * 
// * @param mensaje			El mensaje a mostrar
// * @param segundosTest		Los segundos de duración de la prueba
// */
//void Utils_testBoardWithLed(const char *mensaje, DWORD segundosTest);

/**
 * Resetea la placa
 */
void Utils_reset(void);

/**
 * Intenta extraer un WORD desde un String ASCII-Z
 * 
 * @param data				El array de caracteres del que extraer
 * @param value				El puntero donde escribir el valor
 * @return					¿Se ha podido extraer?
 */
BYTE Utils_extractWordAsciiZ(BYTE *data, WORD *value);

/**
 * Intenta extraer un DWORD desde un String ASCII-Z
 * 
 * @param data				El array de caracteres del que extraer
 * @param value				El puntero donde escribir el valor
 * @return					¿Se ha podido extraer?
 */
BYTE Utils_extractDwordAsciiZ(BYTE *data, DWORD *value);

/**
 * Intenta extraer un int32 desde un String ASCII-Z
 * 
 * @param data				El array de caracteres del que extraer
 * @param value				El puntero donde escribir el valor
 * @return					¿Se ha podido extraer?
 */
BYTE Utils_extractInt32AsciiZ(BYTE *data, int32 *value);

/**
 * Extrae un valor de 0 a 255 de los (hasta) 3 dígitos ASCII que hay a partir
 * de un puntero
 * 
 * @param data				El array de datos
 * @param numDigits			El número de dígitos (de 1 a 3)
 * @return					El valor (0xFFFF si no se puede "parsear" o si el valor se sale del rango)
 */
WORD Utils_getValueFrom0to255ascii(BYTE *data, BYTE numDigits);

/**
 * Extrae un valor de 0 a 1023 de los (hasta) 4 dígitos ASCII que hay a partir
 * de un puntero
 * 
 * @param data				El array de datos
 * @param numDigits			El número de dígitos (de 1 a 4)
 * @return					El valor (0xFFFF si no se puede "parsear" o si el valor se sale del rango)
 */
WORD Utils_getValueFrom0to1023ascii(BYTE *data, WORD numDigits);

/**
 * Extrae un valor de 0 a 65535 de los (hasta) 5 dígitos ASCII que hay a partir
 * de un puntero
 * 
 * @param data				El array de datos
 * @param numDigits			El número de dígitos (de 1 a 5)
 * @return					El valor (0xFFFFFFFF si no se puede "parsear" o si el valor se sale del rango)
 */
DWORD Utils_getValueFrom0to65535ascii(BYTE *data, WORD numDigits);

/**
 * Intenta extraer un float desde un String ASCII-Z
 * 
 * @param data				El array de caracteres del que extraer
 * @param value				El puntero donde escribir el valor
 * @return					¿Se ha podido extraer?
 */
BYTE Utils_extractFloatAsciiZ(BYTE *data, float *value);

/**
 * Intenta extraer un array de bytes que llegan en formato ASCII-HEX, 2 dígitos por byte (por ejemplo 0x1234 nos llegaría como "1234")
 * 
 * @param src				Los datos recibidos
 * @param srcLen			El número de bytes recibidos
 * @param maxBytes			El número máximo de bytes a escribir en el destino, ya convertidos
 * @param dest				El búffer donde escribir la clave
 * @return					El número de bytes extraídos, y 0xFFFF si el formato es incorrecto
 */
WORD Utils_extractAsciiHexData(BYTE *src, WORD srcLen, WORD maxBytes, BYTE *dest);

/**
 * Intenta extraer un nombre DNS (ASCII-Z)
 * 
 * @param src				Los datos recibidos
 * @param srcLen			El número de bytes recibidos
 * @param dest				El destino donde guardarlo
 * @param destLen			La longitud máxima del destino, incluyendo el NULL
 * @return					TRUE si lo ha podido extraer y FALSE en caso contrario
 */
BYTE Utils_extractDnsName(BYTE *src, WORD srcLen, char *dest, WORD destLen);

/**
 * Le hace el swap a un array de bytes de longitud variable
 * 
 * @param destino			El destino
 * @param origen			El origen
 * @param length			La longitud en bytes
 */
void Utils_swapEndianess(BYTE *destino, BYTE *origen, WORD length);

/**
 * Imprime N copias del mismo carácter
 * 
 * @param dato				El dato a imprimir
 * @param numCopias			El valor de N
 */
void Utils_printMultipleCopiesOfChar(char dato, WORD numCopias);


#define printTime(x)                                         \
    do {                                                     \
        QWORD timeStamp = TimerFake_getCurrentMilis();       \
        x;                                                   \
        timeStamp = TimerFake_getCurrentMilis() - timeStamp; \
                                                             \
        printf("%s time %llu ms\n", __func__, timeStamp);    \
    } while (0)

#define printIpv4(str,x) printf("%s %u.%u.%u.%u\n",str,(x).v[0],(x).v[1],(x).v[2],(x).v[3])
#define printIpv6(str,x) printf("%s %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n",str, \
                    (x).in6_u.u6_addr8[0],                                                                          \
                    (x).in6_u.u6_addr8[1],                                                                          \
                    (x).in6_u.u6_addr8[2],                                                                          \
                    (x).in6_u.u6_addr8[3],                                                                          \
                    (x).in6_u.u6_addr8[4],                                                                          \
                    (x).in6_u.u6_addr8[5],                                                                          \
                    (x).in6_u.u6_addr8[6],                                                                          \
                    (x).in6_u.u6_addr8[7],                                                                          \
                    (x).in6_u.u6_addr8[8],                                                                          \
                    (x).in6_u.u6_addr8[9],                                                                          \
                    (x).in6_u.u6_addr8[10],                                                                         \
                    (x).in6_u.u6_addr8[11],                                                                         \
                    (x).in6_u.u6_addr8[12],                                                                         \
                    (x).in6_u.u6_addr8[13],                                                                         \
                    (x).in6_u.u6_addr8[14],                                                                         \
                    (x).in6_u.u6_addr8[15])
//=================================================================================================

#endif
