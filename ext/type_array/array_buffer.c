#include <type_array_ext.h>

VALUE rb_cArrayBuffer;

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

VALUE rb_alloc_array_buffer(unsigned long length, void *data)
{
    VALUE buffer;
    rb_array_buffer_t *buf = NULL;
    if (length == 0) rb_raise(rb_eRangeError, "ArrayBuffer is not a small enough positive integer");
    buffer = Data_Make_Struct(rb_cArrayBuffer, rb_array_buffer_t, 0, rb_free_array_buffer, buf);
    buf->length = length;
    buf->buf = xcalloc(buf->length, 1);
    if (!buf->buf) rb_raise(rb_eRangeError, "Unable to allocate ArrayBuffer");
    if (data) memmove(buf->buf, data, length);
    rb_obj_call_init(buffer, 0, NULL);
    return buffer;
}

static VALUE rb_copy_array_buffer(rb_array_buffer_t *source, long begin, long end)
{
    VALUE buffer;
    long length;
    length = (end - begin);
    if (length < 0) length = 0;
    buffer = rb_alloc_array_buffer(length, NULL);
    GetArrayBuffer(buffer);
    memmove((source + begin), buf->buf, length);
    return buffer;
}

static VALUE rb_array_buffer_s_new(VALUE klass, VALUE obj)
{
    VALUE buffer;
    if (FIXNUM_P(obj)) { // Length constructor
        buffer = rb_alloc_array_buffer(FIX2ULONG(obj), NULL);
    } else if (rb_type(obj) == T_STRING) { // String constructor
        ArrayBufferEncode(obj);
        buffer = rb_alloc_array_buffer((unsigned long)RSTRING_LEN(obj), (void *)RSTRING_PTR(obj));
    } else {
        rb_raise(rb_eTypeError, "ArrayBuffer constructor %s not supported.", RSTRING_PTR(rb_obj_as_string(obj)));
    }
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

VALUE rb_array_buffer_to_s(VALUE obj)
{
    VALUE str;
    GetArrayBuffer(obj);
    str = rb_str_new((const char*)buf->buf, buf->length);
    ArrayBufferEncode(str);
    return rb_obj_freeze(str);
}

void _init_array_buffer()
{
    rb_cArrayBuffer = rb_define_class("ArrayBuffer", rb_cObject);

    rb_define_singleton_method(rb_cArrayBuffer, "new", rb_array_buffer_s_new, 1);

    rb_define_method(rb_cArrayBuffer, "byte_length", rb_array_buffer_byte_length, 0);
    rb_define_method(rb_cArrayBuffer, "slice", rb_array_buffer_slice, -1);
    rb_define_method(rb_cArrayBuffer, "to_s", rb_array_buffer_to_s, 0);
}