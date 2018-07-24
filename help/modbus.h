#ifndef MODBUS_H_
#define	MODBUS_H_

#include "../bajoNivel/tipos.h"

//=================================================================================================

/* Posibles valores de respuesta para los "MODBUS_RESPONSE_STATUS" (ver por ejemplo
 * en el fichero "commModbusRS485.h", el primer parámetro de las funciones "onReceive")
 */
#define MODBUS_RESPONSE_STATUS_OK											0x00
#define MODBUS_RESPONSE_STATUS_TIMEOUT										0xFF
#define MODBUS_RESPONSE_STATUS_UNEXPECTED_FORMAT							0xFE
#define MODBUS_RESPONSE_STATUS_ILLEGAL_FUNCTION								0x01
#define MODBUS_RESPONSE_STATUS_ILLEGAL_DATA_ADDRESS							0x02
#define MODBUS_RESPONSE_STATUS_ILLEGAL_DATA_VALUE 							0x03
#define MODBUS_RESPONSE_STATUS_SERVER_DEVICE_FAILURE 						0x04
#define MODBUS_RESPONSE_STATUS_ACKNOWLEDGE 									0x05
#define MODBUS_RESPONSE_STATUS_SERVER_DEVICE_BUSY 							0x06
#define MODBUS_RESPONSE_STATUS_MEMORY_PARITY_ERROR 							0x08
#define MODBUS_RESPONSE_STATUS_GATEWAY_PATH_UNAVAILABLE 					0x0A
#define MODBUS_RESPONSE_STATUS_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND 		0x0B

//=================================================================================================

// funciones aplicables dentro del protocolo MODBUS
#define MODBUS_FUNCTION_READ_COILS											1
#define MODBUS_FUNCTION_READ_INPUT_DISCRETES								2
#define MODBUS_FUNCTION_READ_MULTIPLE_REGISTERS								3	// READ_HOLDING_REGISTERS
#define MODBUS_FUNCTION_READ_INPUT_REGISTERS								4
#define MODBUS_FUNCTION_WRITE_COIL											5
#define MODBUS_FUNCTION_WRITE_SINGLE_REGISTER								6	// WRITE_SINGLE_HOLDING_REGISTER
#define MODBUS_FUNCTION_WRITE_MULTIPLE_COILS								15
#define MODBUS_FUNCTION_WRITE_MULTIPLE_REGISTERS							16	// WRITE_HOLDING_REGISTERS

/* para aquellos casos en los que se da un error (una excecpión) en la respuesta el "codigoFuncion"
 * es el mismo que en la petición (igual que ocurre cuando va bien), pero activando el bit superior,
 * o sea, haciendo un OR con 0x80
 */
#define MODBUS_FUNCTION_ERROR_INDICATOR										0x80

/* es el número de bytes a descontar del sizeof del paquete correspondiente a la hora de calcular el valor
 * que se debe usar en el campo "length" (es la suma de los 3 primeros WORD's de la HEADER)
 */
#define MODBUS_TCP_LONGITUD_HEADER_A_DESCONTAR								(3 * sizeof(WORD))

//===================================== MODBUS-TCP ================================================

/* Formato general de una trama MODBUS-TCP:
 * - idTransaccion (WORD)
 * - idProtocolo (WORD)
 * - length (WORD)
 * - mensaje MODBUS-RTU, sin incluir el CRC (se elimina dado que ya hay control de errores en TCP)
 */

// en la cabecera MODBUS_TCP siempre se establece el valor 0 como "ID de protocolo" que va encapsulado dentro
#define MODBUS_TCP_ID_PROTOCOLO_IN_TCP_HEADER								0

// parte de la cabecera que va añadida antes del campo "unitId"
typedef struct {
	WORD idTransaccion;
	WORD idProtocolo;	// es siempre 0x0000
	WORD length;		// de lo que va después de este campo (no se incluyen los 2 bytes del propio campo)
} __attribute__ ((packed)) MODBUS_TCP_MINI_HEADER;

// datos que van siempre en MODBUS-TCP
typedef struct {
	WORD idTransaccion;
	WORD idProtocolo;	// es siempre 0x0000
	WORD length;		// de lo que va después de este campo (no se incluyen los 2 bytes del propio campo)

	BYTE idUnit;
	BYTE codigoFuncion;
} __attribute__ ((packed)) MODBUS_TCP_HEADER;

// formato completo para el caso de ReadInputDiscretes (2) (TCP-REQUEST)
typedef struct {
	MODBUS_TCP_HEADER header;

	WORD offset;
	WORD numWords;
} __attribute__ ((packed)) MODBUS_READ_INPUT_DISCRETES_TCP_REQUEST;

// formato completo para el caso de ReadMultipleRegisters (3) (TCP-REQUEST)
typedef MODBUS_READ_INPUT_DISCRETES_TCP_REQUEST MODBUS_READ_MULTIPLE_REGISTERS_TCP_REQUEST;

// formato completo para el caso de ReadInputRegisters (4) (TCP-REQUEST)
typedef struct {
	MODBUS_TCP_HEADER header;

	WORD offset;
	WORD numWords;
} __attribute__ ((packed)) MODBUS_READ_INPUT_REGISTERS_TCP_REQUEST;

// formato completo para el caso de WriteSingleRegister (6) (TCP-REQUEST)
typedef struct {
	MODBUS_TCP_HEADER header;

	WORD offset;
	WORD value;
} __attribute__ ((packed)) MODBUS_WRITE_SINGLE_REGISTER_TCP_REQUEST;

// formato completo para el caso de WriteMultipleRegisters (16) (TCP-REQUEST)
typedef struct {
	MODBUS_TCP_HEADER header;

	WORD offset;
	WORD numWords;
	BYTE numBytes;	// numBytes = 2 x numWords
	// después van los valores a escribir
} __attribute__ ((packed)) MODBUS_WRITE_MULTIPLE_REGISTERS_TCP_REQUEST;

// formato completo para el caso de ReadInputDiscretes (2) (TCP-RESPONSE)
typedef struct {
	MODBUS_TCP_HEADER header;

	BYTE numBytes;	// numBytes = número de bytes necesarios para codificar las Inputs pedidas en la petición
	// después van los datos
} __attribute__ ((packed)) MODBUS_READ_INPUT_DISCRETES_TCP_RESPONSE;

// formato completo para el caso de ReadInputRegisters (4) (TCP-RESPONSE)
typedef struct {
	MODBUS_TCP_HEADER header;

	BYTE numBytes;	// numBytes = 2 x numWords (de la petición)
	// después van los datos
} __attribute__ ((packed)) MODBUS_READ_INPUT_REGISTERS_TCP_RESPONSE;

// formato completo para el caso de ReadMultipleRegisters (3) (TCP-RESPONSE)
typedef MODBUS_READ_INPUT_REGISTERS_TCP_RESPONSE MODBUS_READ_MULTIPLE_REGISTERS_TCP_RESPONSE;

// formato completo para el caso de WriteSingleRegister (6) (TCP-RESPONSE)
typedef struct {
	MODBUS_TCP_HEADER header;

	WORD offset;
	WORD value;
} __attribute__ ((packed)) MODBUS_WRITE_SINGLE_REGISTER_TCP_RESPONSE;

// formato completo para el caso de WriteMultipleRegisters (16) (TCP-RESPONSE)
typedef struct {
	MODBUS_TCP_HEADER header;

	WORD offset;
	WORD numWords;
	// después van los datos
} __attribute__ ((packed)) MODBUS_WRITE_MULTIPLE_REGISTERS_TCP_RESPONSE;

// formato de un paquete de error
typedef struct {
	MODBUS_TCP_HEADER header;
	
	BYTE exceptionCode;
} __attribute__ ((packed)) MODBUS_ERROR_TCP_RESPONSE;

// en el caso de otras respuestas sin payload la longitud mínima no es inferior
#define MODBUS_TCP_RESPONSE_MIN_LENGTH			sizeof(MODBUS_ERROR_TCP_RESPONSE)

//===================================== MODBUS-RTU ================================================

// parte inicial de una petición en MODBUS-RTU
typedef struct {
	BYTE idUnit;
	BYTE codigoFuncion;
} __attribute__ ((packed)) MODBUS_RTU_HEADER;

// formato completo para el caso de ReadCoils (1) (RTU-REQUEST)
typedef struct {
	MODBUS_RTU_HEADER header;

	WORD offset;
	WORD numWords;
	WORD crc;
} __attribute__ ((packed)) MODBUS_READ_COILS_RTU_REQUEST;

// formato completo para el caso de ReadInputDiscretes (2) (RTU-REQUEST)
typedef struct {
	MODBUS_RTU_HEADER header;

	WORD offset;
	WORD numWords;
	WORD crc;
} __attribute__ ((packed)) MODBUS_READ_INPUT_DISCRETES_RTU_REQUEST;

// formato completo para el caso de ReadMultipleRegisters (3) (RTU-REQUEST)
typedef MODBUS_READ_INPUT_DISCRETES_RTU_REQUEST MODBUS_READ_MULTIPLE_REGISTER_RTU_REQUEST;

// formato completo para el caso de ReadInputRegisters (4) (RTU-REQUEST)
typedef struct {
	MODBUS_RTU_HEADER header;

	WORD offset;
	WORD numWords;
	WORD crc;
} __attribute__ ((packed)) MODBUS_READ_INPUT_REGISTERS_RTU_REQUEST;

// formato completo para el caso de WriteSingleRegister (6) (RTU-REQUEST)
typedef struct {
	MODBUS_RTU_HEADER header;

	WORD offset;
	WORD value;
	WORD crc;
} __attribute__ ((packed)) MODBUS_WRITE_SINGLE_REGISTER_RTU_REQUEST;

// formato completo para el caso de WriteMultipleRegisters (16) (RTU-REQUEST)
typedef struct {
	MODBUS_RTU_HEADER header;

	WORD offset;
	WORD numWords;
	BYTE numBytes;
	// después van los datos y finalmente el CRC
} __attribute__ ((packed)) MODBUS_WRITE_MULTIPLE_REGISTERS_RTU_REQUEST;

// formato de una respuesta a la función read multiples registers
typedef struct {
	MODBUS_RTU_HEADER header;

	BYTE byteCount;

	// después van los datos y el crc
} __attribute__ ((packed)) MODBUS_RTU_READ_MULTIPLE_REGISTERS_HEADER_RESPONSE;

// formato de una respuesta para el caso de ReadInputDiscretes (2) (RTU-RESPONSE)
typedef struct {
	MODBUS_RTU_HEADER header;

	BYTE numBytes;
	// después van los bytes (codificando los bits de las entradas pedidas), y al final va el CRC (WORD)
} __attribute__ ((packed)) MODBUS_READ_INPUT_DISCRETES_RTU_RESPONSE;

// formato de una respuesta para el caso de ReadInputRegisters (4) (RTU-RESPONSE)
typedef struct {
	MODBUS_RTU_HEADER header;

	BYTE byteCount;
	// después van los bytes, y al final va el CRC (WORD)
} __attribute__ ((packed)) MODBUS_READ_INPUT_REGISTERS_RTU_RESPONSE;

// formato de una respuesta para el caso de WriteSingleRegister (6) (RTU-RESPONSE)
typedef MODBUS_WRITE_SINGLE_REGISTER_RTU_REQUEST MODBUS_WRITE_SINGLE_REGISTER_RTU_RESPONSE;

// formato de una respuesta para el caso de WriteMultipleRegisters (16) (RTU-RESPONSE)
typedef struct {
	MODBUS_RTU_HEADER header;

	WORD offset;
	WORD numWords;
	WORD crc;
} __attribute__ ((packed)) MODBUS_WRITE_MULTIPLE_REGISTERS_RTU_RESPONSE;

// formato de un paquete de error
typedef struct {
	MODBUS_RTU_HEADER header;
	
	BYTE exceptionCode;
	WORD crc;
} __attribute__ ((packed)) MODBUS_ERROR_RTU_RESPONSE;

// en el caso de otras respuestas sin payload la longitud mínima no es inferior
#define MODBUS_RTU_RESPONSE_MIN_LENGTH			(sizeof(MODBUS_READ_INPUT_REGISTERS_RTU_RESPONSE) + 2)

//=================================================================================================

/**
 * Calcula el checsum MODBUS para un array de datos
 *
 * @param data				El array de datos
 * @param len				El número de bytes
 * @return					El checksum
 */
WORD Modbus_crc16(BYTE *data, WORD len);

//=================================================================================================

#endif	
