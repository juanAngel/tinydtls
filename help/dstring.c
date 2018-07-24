#include "dstring.h"

void dstring_init(DString *ds, uint16_t maxSize){
    ds->max = maxSize;
    ds->len = 0;
    ds->buff = NULL;
    if(maxSize){
        ds->buff = (char*)malloc(maxSize+1);
        ds->buff[0] = 0;
    }
}

DString *dstring_new_size(const char *str, uint16_t len){
    DString* ret = (DString*)malloc(sizeof (DString));

    dstring_init(ret,0);
    dstring_appened_len(ret,str,len);

    return ret;
}
void dstring_free(DString *ds){
    dstring_clean(ds);
    free(ds);
}

DString *dstring_appened(DString *ds, const char *str){
    return dstring_appened_len(ds,str,(uint16_t)-1);
}

DString *dstring_appened_len(DString *ds, const char *str, uint16_t len){
    uint32_t i = 0;
    while (str[i] && len) {
        dstring_appened_c(ds,str[i]);
        ++i;
        --len;
    }
    return ds;
}

DString *dstring_truncate(DString *ds, uint16_t len){
    ds->len = len<ds->len?len:ds->len;
    ds->buff[ds->len] = 0;

    return ds;
}
DString *dstring_appened_c(DString *ds, const char c){
    if(c){
        uint32_t len = ds->len++;
        if(ds->max<ds->len){
            if(ds->max){
                ds->max *=2;
                ds->buff = (char*)realloc(ds->buff,ds->max);
            }else{
                ds->buff = (char*)malloc(8);
            }
        }
        ds->buff[len] = c;
        ds->buff[ds->len] = 0;
    }
    return  ds;
}

int dstring_cmp(DString *ds1, DString *ds2){
    if(ds1->len == ds2->len){
        int i = 0;
        while (ds1->buff[i] && ds1->buff[i] == ds2->buff[i]) {
            i++;
        }
        return ds1->buff[i] - ds2->buff[i];
    }

    return ds1->len - ds2->len;
}

int dstring_cmp_cstring(DString *ds1, const char *str){int i = 0;
    while (ds1->buff[i] && ds1->buff[i] == str[i]) {
       i++;
    }
    return ds1->buff[i] - str[i];
}



void dstring_clean(DString *ds){
    free(ds->buff);
    ds->buff = NULL;
    ds->len = ds->max = 0;
}
