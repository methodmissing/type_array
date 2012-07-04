#ifndef TYPE_ARRAY_H
#define TYPE_ARRAY_H

typedef struct {
    unsigned long size;
    unsigned long length;
    unsigned long byte_length;
    unsigned long byte_offset;
    VALUE buf;
} rb_type_array_t;

#define GetTypeArray(obj) \
    rb_type_array_t *ary = NULL; \
    Data_Get_Struct(obj, rb_type_array_t, ary); \
    if (!ary) rb_raise(rb_eTypeError, "uninitialized TypeArray!");

inline long rb_type_array_aset(VALUE obj, VALUE idx, rb_type_array_t *ary, VALUE item);
inline long rb_type_array_aget(VALUE obj, VALUE idx, rb_type_array_t *ary);
void rb_type_array_swizzle(char* buf, unsigned long len);
void _init_type_array();

#endif