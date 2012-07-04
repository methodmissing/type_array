#ifndef TYPE_ARRAY_H
#define TYPE_ARRAY_H

typedef VALUE (type_array_coerce_rb_fn) (rb_array_buffer_t *buf, long index);
typedef void (type_array_coerce_c_fn) (rb_array_buffer_t *buf, long index, VALUE item);

typedef struct {
    unsigned long size;
    unsigned long length;
    unsigned long byte_length;
    unsigned long byte_offset;
    type_array_coerce_rb_fn *c_to_rb;
    type_array_coerce_c_fn *rb_to_c;
    VALUE buf;
} rb_type_array_t;

#define GetTypeArray(obj) \
    rb_type_array_t *ary = NULL; \
    Data_Get_Struct(obj, rb_type_array_t, ary); \
    if (!ary) rb_raise(rb_eTypeError, "uninitialized TypeArray!");

inline VALUE rb_type_array_coerce(VALUE obj, void *val);
inline long rb_type_array_aset0(VALUE idx, rb_type_array_t *ary, VALUE item);
inline long rb_type_array_aget0(VALUE idx, rb_type_array_t *ary);
void rb_type_array_swizzle(char* buf, unsigned long len);
void _init_type_array();

#endif