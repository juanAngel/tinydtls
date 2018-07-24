#include "dbytearray.h"
#include <string.h>


size_t calcSize(size_t s){
    if(s){
        size_t size = 8;
        int i;
        for(i = 0;i<(sizeof(s)*8 -3) && size<s;i++){
            size<<=1;
        }
        return size;
    }
    return 0;
}

static void* forwar_malloc(size_t s){
    return malloc(calcSize(s));
}
static void* forwar_realloc(void* ptr,size_t s){
    return realloc(ptr,calcSize(s));
}
#define malloc(s) forwar_malloc(s)
#define realloc(ptr,s) forwar_realloc(ptr,s)

void dbytearray_init(DArray *a, uint16_t len){
    if(a){
        a->len = len;
        a->buff = NULL;
        if(len){
            a->buff = (uint8_t*)malloc(len);
            memset(a->buff,0,len);
        }
    }
}

void dbytearray_clean(DArray *a){
    if(a){
        a->len = 0;
        if(a->buff)
            free(a->buff);
    }
}

DArray *dbytearray_new(uint16_t len){
    DArray *ret = (DArray*)malloc(sizeof (DArray));

    dbytearray_init(ret,len);

    return ret;
}

void dbytearray_free(DArray *a){
    dbytearray_clean(a);
    free(a);
}

DArray *dbytearray_appened(DArray *a,const uint8_t *data, uint16_t len){
    unsigned index = a->len,i = 0;
    dbytearray_setSize(a,a->len+len);

    for(;i<len;i++)
        a->buff[index+i] = data[i];

    return a;
}

DArray *dbytearray_setSize(DArray *a, uint16_t newLen){
    if(newLen){
        a->buff = (uint8_t*)realloc(a->buff,newLen);
        a->len = newLen;
    }else{
        a->len = 0;
        free(a->buff);
        a->buff = NULL;
    }
    return  a;
}

DArray *dbytearray_remove(DArray *a, uint16_t index, uint16_t len){
    if(a){
        unsigned i;
        for(i = index;(i+len)<a->len;i++){
            a->buff[i] = a->buff[i+len];
        }
        dbytearray_setSize(a,a->len-len);
    }
    return a;
}
