#ifndef CRC16_H
#define CRC16_H

#include <stdlib.h>
#include <stdint.h>

uint16_t crc16_update(uint16_t crc, uint8_t a);
inline uint16_t crc16_calc(uint8_t* data,size_t size){
    uint16_t crc = 0;
    size_t i;

    for(i = 0;i<size;i++){
        crc = crc16_update(crc,data[i]);
    }

    return crc;
}


#endif //CRC16_H
