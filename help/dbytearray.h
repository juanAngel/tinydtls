#ifndef DARRAY_H
#define  DARRAY_H


#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
  union {
    uint8_t *buff;
    uint8_t *data;
  };
  uint16_t len;
} DArray;

typedef DArray *darray_t;

void    dbytearray_init(DArray  *a,
                        uint16_t len);
void    dbytearray_clean(DArray *a);
DArray* dbytearray_new(uint16_t len);
#define darray_new_zt(len) darray_new(len + 1)
void    dbytearray_free(DArray *a);
#define dptrarray_free(a) dbytearray_free((DArray *)(a))

DArray* dbytearray_appened(DArray  *a,
                           const uint8_t *data,
                           uint16_t len);
DArray* dbytearray_remove(DArray  *a,
                          uint16_t index,
                          uint16_t len);
#define darray_remove_byte(a, index) dbytearray_remove(a, index, 1)

#define darray_appened_vals(a, value, n) dbytearray_appened(a,                    \
                                                            ((uint8_t *)& value), \
                                                            sizeof(value) *n)
#define darray_appened_val(a, value) dbytearray_appened(a,                  \
                                                        (uint8_t *)& value, \
                                                        sizeof(value))
#define darray_pop_val(a, type) dbytearray_remove(a, -1, sizeof(type))
#define darray_appened_lval(a, value) do{ \
    void *val = (void *)(value);        \
    darray_appened_val(a, val);         \
}while(0)
DArray* dbytearray_setSize(DArray  *a,
                           uint16_t newLen);

#define darray_index(a, t, i) (((t *)(void *)(a)->data)[(i)])
#define darray_getLength(a, t) ((a)->len / sizeof(t))
#define darray_setLength(a, t, n) dbytearray_setSize(a, sizeof(t) * n)

// #define dptrarray_index(array,index_) ((array)->pdata)[index_]
#endif // DARRAY_H
