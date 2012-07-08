#ifndef TYPE_ARRAY_EXT_H
#define TYPE_ARRAY_EXT_H

#include "ruby.h"
#include <prelude.h>

extern VALUE rb_cArrayBuffer;

extern VALUE rb_cTypeArray;
extern VALUE rb_cInt8Array;
extern VALUE rb_cUInt8Array;
extern VALUE rb_cInt16Array;
extern VALUE rb_cUInt16Array;
extern VALUE rb_cInt32Array;
extern VALUE rb_cUInt32Array;
extern VALUE rb_cInt64Array;
extern VALUE rb_cUInt64Array;

extern VALUE rb_cDataView;

void rb_type_array_set_int8(void *abuf, unsigned long index, char val, VALUE swiz);
void rb_type_array_set_uint8(void *abuf, unsigned long index, unsigned char val, VALUE swiz);
void rb_type_array_set_int16(void *abuf, unsigned long index, short val, VALUE swiz);
void rb_type_array_set_uint16(void *abuf, unsigned long index, unsigned short val, VALUE swiz);
void rb_type_array_set_int32(void *abuf, unsigned long index, int val, VALUE swiz);
void rb_type_array_set_uint32(void *abuf, unsigned long index, unsigned int val, VALUE swiz);
void rb_type_array_set_float32(void *abuf, unsigned long index, float val, VALUE swiz);
void rb_type_array_set_float64(void *abuf, unsigned long index, double val, VALUE swiz);

char rb_type_array_get_int8(void *abuf, unsigned long index, VALUE swiz);
unsigned char rb_type_array_get_uint8(void *abuf, unsigned long index, VALUE swiz);
short rb_type_array_get_int16(void *abuf, unsigned long index, VALUE swiz);
unsigned short rb_type_array_get_uint16(void *abuf, unsigned long index, VALUE swiz);
int rb_type_array_get_int32(void *abuf, unsigned long index, VALUE swiz);
unsigned int rb_type_array_get_uint32(void *abuf, unsigned long index, VALUE swiz);
float rb_type_array_get_float32(void *abuf, unsigned long index, VALUE swiz);
double rb_type_array_get_float64(void *abuf, unsigned long index, VALUE swiz);

#define DefineTypeGetter(name, type) \
    inline type rb_type_array_get_##name(void *abuf, unsigned long index, VALUE swiz) \
    { \
        char buf[sizeof(type)]; \
        type val; \
        memmove(buf, (char *)abuf + index, sizeof(type)); \
        if (swiz) rb_type_array_swizzle(buf, sizeof(type)); \
        memmove(&val, buf, sizeof(type)); \
        return val; \
    }

#define DefineTypeSetter(name, type) \
    inline void rb_type_array_set_##name(void *abuf, unsigned long index, type val, VALUE swiz) \
    { \
        char buf[sizeof(type)]; \
        memmove(buf, &val, sizeof(type)); \
        if (swiz) rb_type_array_swizzle(buf, sizeof(type)); \
        memmove((char *)abuf + index, buf, sizeof(type)); \
    }

#define DefineTypeAccessor(name, type) \
    DefineTypeSetter(name, type); \
    DefineTypeGetter(name, type);

#include <array_buffer.h>
#include <type_array.h>
#include <data_view.h>

#endif