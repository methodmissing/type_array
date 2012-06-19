#ifndef DATA_VIEW_H
#define DATA_VIEW_H

typedef struct {
    unsigned long byte_length;
    unsigned long byte_offset;
    VALUE buf;
} rb_data_view_t;

#define GetDataView(obj) \
    rb_data_view_t *view = NULL; \
    Data_Get_Struct(obj, rb_data_view_t, view); \
    if (!view) rb_raise(rb_eTypeError, "uninitialized DataView!");

#define DataViewAset(obj, idx) \
    VALUE offset, item, little_endian; \
    unsigned long byte_offset; \
    GetDataView(obj); \
    GetArrayBuffer(view->buf); \
    rb_scan_args(argc, argv, "21", &offset, &item, &little_endian); \
    if (NIL_P(little_endian)) little_endian = Qfalse; \
    byte_offset = FIX2ULONG(offset); \
    if (offset < 0) rb_raise(rb_eRangeError, "Offset may not be negative."); \
    if (offset > view->byte_length) rb_raise(rb_eRangeError, "Offset out of range.");

#define DataViewAget(obj, idx) \
    VALUE offset, little_endian; \
    unsigned long byte_offset; \
    GetDataView(obj); \
    GetArrayBuffer(view->buf); \
    rb_scan_args(argc, argv, "11", &offset, &little_endian); \
    if (NIL_P(little_endian)) little_endian = Qfalse; \
    byte_offset = FIX2ULONG(offset); \
    if (offset < 0) rb_raise(rb_eRangeError, "Offset may not be negative."); \
    if (offset > view->byte_length) rb_raise(rb_eRangeError, "Offset out of range.");

void _init_data_view();

#endif