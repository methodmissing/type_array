#include <type_array_ext.h>

VALUE rb_cArrayBuffer;

static ID rb_array_buffer_intern_rewind;

/*
 * :nodoc:
 *  GC free callback
 *
*/
static void rb_free_array_buffer(void *ptr)
{
    rb_array_buffer_t *buf = (rb_array_buffer_t *)ptr;
    if (buf) {
        if (buf->buf) xfree(buf->buf);
        xfree(buf);
        buf = NULL;
    }
}

VALUE rb_alloc_array_buffer(unsigned long length)
{
    VALUE buffer;
    rb_array_buffer_t *buf = NULL;
    if (length == 0) rb_raise(rb_eRangeError, "ArrayBuffer is not a small enough positive integer");
    buffer = Data_Make_Struct(rb_cArrayBuffer, rb_array_buffer_t, 0, rb_free_array_buffer, buf);
    buf->length = length;
    buf->buf = xcalloc(buf->length, 1);
    if (!buf->buf) rb_raise(rb_eRangeError, "Unable to allocate ArrayBuffer");
    rb_obj_call_init(buffer, 0, NULL);
    return buffer;
}

static VALUE rb_copy_array_buffer(rb_array_buffer_t *source, long begin, long end)
{
    VALUE buffer;
    long length;
    length = (end - begin);
    if (length < 0) length = 0;
    buffer = rb_alloc_array_buffer(length);
    GetArrayBuffer(buffer);
    memmove((source + begin), buf->buf, length);
    return buffer;
}

static VALUE rb_array_buffer_s_new(VALUE klass, VALUE len)
{
    VALUE buffer;
    Check_Type(len, T_FIXNUM);
    buffer = rb_alloc_array_buffer(FIX2ULONG(len));
    return buffer;
}

static VALUE rb_array_buffer_s_read(VALUE klass, VALUE io, VALUE len)
{
    VALUE buffer;
    rb_io_t *fptr;
    Check_Type(len, T_FIXNUM);
    rb_funcall(io, rb_array_buffer_intern_rewind, 0);
    buffer = rb_alloc_array_buffer(FIX2ULONG(len));
    GetArrayBuffer(buffer);
    GetOpenFile(io, fptr);
    rb_io_check_readable(fptr);
    FILE *f = GetReadFile(fptr);
    rb_io_fread((char *)buf->buf, (long)buf->length, f);
    return buffer;
}

static VALUE rb_array_buffer_byte_length(VALUE obj)
{
    GetArrayBuffer(obj);
    return ULONG2NUM(buf->length);   
}

static VALUE rb_array_buffer_slice(int argc, VALUE *argv, VALUE obj)
{
    VALUE buffer, begin, end;
    long b = 0;
    long e = 0;
    GetArrayBuffer(obj);
    rb_scan_args(argc, argv, "11", &begin, &end);
    Check_Type(begin, T_FIXNUM);
    b = FIX2LONG(begin);
    if (NIL_P(end)) {
        e = buf->length;
    } else {
        Check_Type(end, T_FIXNUM);
        e = FIX2LONG(end);
    }
    if (b < 0) b = buf->length - abs(b);
    if (e < 0) e = buf->length - abs(e);

    if (e > buf->length) rb_raise(rb_eRangeError, "Offset out of range.");
    buffer = rb_copy_array_buffer(buf, b, e);
    return buffer;
}

VALUE rb_array_buffer_write(VALUE obj, VALUE io)
{
    rb_io_t *fptr;
    GetArrayBuffer(obj);
    GetOpenFile(io, fptr);
    rb_io_check_writable(fptr);
    FILE *f = GetWriteFile(fptr);
    return LONG2NUM(rb_io_fwrite(buf->buf, buf->length, f));
}

void _init_array_buffer()
{
    rb_cArrayBuffer = rb_define_class("ArrayBuffer", rb_cObject);

    rb_array_buffer_intern_rewind = rb_intern("rewind");

    rb_define_singleton_method(rb_cArrayBuffer, "new", rb_array_buffer_s_new, 1);
    rb_define_singleton_method(rb_cArrayBuffer, "read", rb_array_buffer_s_read, 2);

    rb_define_method(rb_cArrayBuffer, "byte_length", rb_array_buffer_byte_length, 0);
    rb_define_method(rb_cArrayBuffer, "slice", rb_array_buffer_slice, -1);
    rb_define_method(rb_cArrayBuffer, "write", rb_array_buffer_write, 1);
}