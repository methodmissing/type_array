#ifndef TYPE_ARRAY_H
#define TYPE_ARRAY_H

typedef void (type_array_aset_fn) (rb_array_buffer_t *buf, long index, VALUE item);
typedef VALUE (type_array_aref_fn) (rb_array_buffer_t *buf, long index);

typedef struct {
    unsigned long size;
    unsigned long length;
    unsigned long byte_length;
    unsigned long byte_offset;
    type_array_aset_fn *aset_fn;
    type_array_aref_fn *aref_fn;
    VALUE buf;
} rb_type_array_t;

#define GetTypeArray(obj) \
    rb_type_array_t *ary = NULL; \
    Data_Get_Struct(obj, rb_type_array_t, ary); \
    if (!ary) rb_raise(rb_eTypeError, "uninitialized TypeArray!");

void rb_type_array_swizzle(char* buf, unsigned long len);
void _init_type_array();

#define DefineTypeArraySetter(name, coercion) \
    void rb_type_array_aset_##name(rb_array_buffer_t *buf, long index, VALUE item) \
    { \
        rb_type_array_set_##name(buf, index, coercion, TYPE_ARRAY_IS_LITTLE_ENDIAN); \
    }

#define DefineTypeArrayGetter(name, type, coercion) \
    VALUE rb_type_array_aref_##name(rb_array_buffer_t *buf, long index) \
    { \
        type val = rb_type_array_get_##name(buf, index, TYPE_ARRAY_IS_LITTLE_ENDIAN); \
        return coercion; \
    }

#endif