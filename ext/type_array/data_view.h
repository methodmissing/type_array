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

#define DataViewAset(obj) \
    VALUE offset, item, little_endian; \
    unsigned long index; \
    GetDataView(obj); \
    GetArrayBuffer(view->buf); \
    rb_scan_args(argc, argv, "21", &offset, &item, &little_endian); \
    if (NIL_P(little_endian)) little_endian = Qfalse; \
    index = rb_data_view_assert_offset(view, offset);

#define DataViewAget(obj) \
    VALUE offset, little_endian; \
	unsigned long index; \
    GetDataView(obj); \
    GetArrayBuffer(view->buf); \
    rb_scan_args(argc, argv, "11", &offset, &little_endian); \
    if (NIL_P(little_endian)) little_endian = Qfalse; \
    index = rb_data_view_assert_offset(view, offset);

void _init_data_view();

#endif