#include <type_array_ext.h>

/*
 * :nodoc:
 *  An Array Buffer implementation with a simple query API for it's chunk of managed memory. The buffer is fixed and
 *  cannot be resized. Data View and Type Array instances manipulate the buffer directly (this may change), but none
 *  of that's exposed through a Ruby API.
 *
*/

VALUE rb_cArrayBuffer;

/*
 * :nodoc:
 *  GC free callback
 *
*/
static void rb_free_array_buffer(void *ptr)
{
    rb_array_buffer_t *buf = (rb_array_buffer_t *)ptr;
#ifdef TYPE_ARRAY_DEBUG
    printf(">> rb_free_array_buffer %p\n", ptr);
#endif
    if (buf) {
        if (buf->buf) xfree(buf->buf);
        xfree(buf);
        buf = NULL;
    }
#ifdef TYPE_ARRAY_DEBUG
    printf("<< rb_free_array_buffer %p\n", ptr);
#endif
}

/*
 * :nodoc:
 *  Utility function to allocate a new ArrayBuffer instance of given length. An optional pointer to an existing chunk
 *  of memory may be passed, which becomes the buffer contents. This is useful for reading raw binary data from
 *  standard I/O or sockets as binary strings.
 *
*/
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
    rb_obj_freeze(buffer);
    return buffer;
}

/*
 * :nodoc:
 *  Utility function to create a new ArrayBuffer instance from a begin..end byte range
 *
*/
static VALUE rb_copy_array_buffer(rb_array_buffer_t *source, long begin, long end)
{
    long length;
    void *data;
    length = (end - begin);
    if (length < 0) length = 0;
    data = source + begin;
    return rb_alloc_array_buffer(length, data);
}

/*
 *  call-seq:
 *     ArrayBuffer.new(8)          =>  ArrayBuffer
 *     ArrayBuffer.new("buffer")   =>  ArrayBuffer
 *
 *  Creates a new ArrayBuffer instance. Both length and data (String) constructors are supported.
 *
 * === Examples
 *     buf = ArrayBuffer.new(8)         =>  ArrayBuffer
 *     buf.byte_length                  =>  8
 *
 *     buf = ArrayBuffer.new("buffer")  =>  ArrayBuffer
 *     buf.byte_length                  =>  6
 *     buf.to_s                         =>  "buffer"
 *
*/
static VALUE rb_array_buffer_s_new(VALUE klass, VALUE obj)
{
    VALUE buffer;
    if (FIXNUM_P(obj)) {
        buffer = rb_alloc_array_buffer(FIX2ULONG(obj), NULL);
    } else if (rb_type(obj) == T_STRING) {
        ArrayBufferEncode(obj);
        buffer = rb_alloc_array_buffer((unsigned long)RSTRING_LEN(obj), (void *)RSTRING_PTR(obj));
    } else {
        rb_raise(rb_eTypeError, "ArrayBuffer constructor %s not supported.", RSTRING_PTR(rb_obj_as_string(obj)));
    }
    return buffer;
}

/*
 *  call-seq:
 *     buf.byte_length          =>  Fixnum
 *
 *  Returns the size of the buffer.
 *
 * === Examples
 *     buf = ArrayBuffer.new(8)         =>  ArrayBuffer
 *     buf.byte_length                  =>  8
 *
*/
static VALUE rb_array_buffer_byte_length(VALUE obj)
{
    GetArrayBuffer(obj);
    return ULONG2NUM(buf->length);   
}

/*
 *  call-seq:
 *     buf.slice(1)         =>  ArrayBuffer
 *
 *  Returns a new ArrayBuffer instance with a slice (copy) of a range of memory managed by the source buffer. 
 *
 * === Examples
 *     buf = ArrayBuffer.new(8)         =>  ArrayBuffer
 *     buf.slice(2)                     =>  ArrayBuffer
 *     buf.slice(4,6)                   =>  ArrayBuffer
 *
*/
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

/*
 *  call-seq:
 *     buf.to_s          =>  String
 *
 *  Returns a String (binary) representation of the buffer.
 *
 * === Examples
 *     buf = ArrayBuffer.new("buffer")  =>  ArrayBuffer
 *     buf.byte_length                  =>  6
 *     buf.to_s                         =>  "buffer"
 *
*/
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