#ifndef DSTRING_H
#define DSTRING_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


typedef struct{
    union{
        char* buff;
        char* str;
    };
    uint16_t len,max;
}DString;

void dstring_init(DString* ds, uint16_t maxSize);
void dstring_clean(DString* ds);


#define dstring_new(str) dstring_new_size(str,-1)
DString* dstring_new_size(const char* str,uint16_t len);
static inline DString* dstring_new_empty(uint16_t maxlen){

    DString* ret = (DString*)malloc(sizeof (DString));

    dstring_init(ret,maxlen);

    return ret;
}

void dstring_free(DString* ds);
DString* dstring_appened(DString* ds,const char* str);
DString* dstring_appened_c(DString* ds,const char c);
DString* dstring_appened_len(DString* ds,const char* str,uint16_t len);
DString* dstring_truncate(DString* ds,uint16_t len);
int dstring_cmp(DString* ds1,DString* ds2);
int dstring_cmp_cstring(DString* ds1,const char* str);


#endif //DSTRING_H
