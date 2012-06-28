#ifndef ARRAY_BUFFER_H
#define ARRAY_BUFFER_H

typedef struct {
    unsigned long length;
    void *buf;
} rb_array_buffer_t;

#define GetArrayBuffer(obj) \
    rb_array_buffer_t *buf = NULL; \
    Data_Get_Struct(obj, rb_array_buffer_t, buf); \
    if (!buf) rb_raise(rb_eTypeError, "uninitialized ArrayBuffer!");

VALUE rb_alloc_array_buffer(unsigned long length, void *data);
VALUE rb_array_buffer_write(VALUE obj, VALUE io);

VALUE rb_io_rewind(VALUE io);
void _init_array_buffer();

#endif