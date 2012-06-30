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

#define TypeArrayAset(obj, idx, item) \
    long index; \
    GetTypeArray(obj); \
    GetArrayBuffer(ary->buf); \
    Check_Type(idx, T_FIXNUM); \
    index = FIX2LONG(idx) * ary->size; \
    if (index < 0) rb_raise(rb_eRangeError, "Offset may not be negative."); \
    if (!rb_type_array_assert_alignment(index, ary->size)) rb_raise(rb_eRangeError, "Byte offset is not aligned."); \
    if ((unsigned long)index > ary->byte_length) rb_raise(rb_eRangeError, "Offset out of range."); \
    if (ary->size > (ary->byte_length - (unsigned long)index)) rb_raise(rb_eRangeError, "Offset/length out of range."); \
    switch (TYPE(item)) { \
    case T_FIXNUM: \
    case T_BIGNUM: \
    case T_FLOAT: \
        break; \
    default: \
        rb_raise(rb_eTypeError, "Type arrays only support Fixnum, Bignum and Float instances"); \
    }

#define TypeArrayAget(obj, idx) \
    long index; \
    GetTypeArray((obj)); \
    GetArrayBuffer(ary->buf); \
    Check_Type(idx, T_FIXNUM); \
    index = FIX2LONG(idx) * ary->size; \
    if (index < 0) rb_raise(rb_eRangeError, "Offset may not be negative."); \
    if (!rb_type_array_assert_alignment(index, ary->size)) rb_raise(rb_eRangeError, "Byte offset is not aligned."); \
    if ((unsigned long)index > ary->byte_length) rb_raise(rb_eRangeError, "Offset out of range."); \
    if (ary->size > (ary->byte_length - (unsigned long)index)) rb_raise(rb_eRangeError, "Offset/length out of range."); \

void rb_type_array_swizzle(char* buf, unsigned long len);
void _init_type_array();

#endif