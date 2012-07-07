#include <type_array_ext.h>

/*
 * :nodoc:
 *  Provides a low level view of a given ArrayBuffer instance. This is useful for lower level manipulation of byte streams
 *  and well suited for reading and writing structs / records from and to streams. Expect this to support structs in the
 *  near future. DataView object's underlying buffer's fixed as well - there's no way to manipulate it from Ruby.
 *
*/

VALUE rb_cDataView;

static inline unsigned long rb_data_view_assert_offset(rb_data_view_t *view, VALUE idx)
{
    unsigned long index;
    index = FIX2ULONG(idx); \
    if (index > view->byte_length) rb_raise(rb_eRangeError, "Offset out of range.");
    return index;
}

/*
 * :nodoc:
 *  GC mark callback
 *
*/
static void rb_mark_data_view(void *ptr)
{
    rb_data_view_t *view =  (rb_data_view_t *)ptr;
    if (view) rb_gc_mark(view->buf);
}

/*
 * :nodoc:
 *  GC free callback
 *
*/
void rb_free_data_view(void *ptr)
{
    rb_data_view_t *view = (rb_data_view_t *)ptr;
#ifdef TYPE_ARRAY_DEBUG
    printf(">> rb_free_data_view %p\n", ptr);
#endif
    if (view) {
        xfree(view);
        view = NULL;
    }
#ifdef TYPE_ARRAY_DEBUG
    printf("<< rb_free_data_view %p\n", ptr);
#endif
}

/*
 *  call-seq:
 *     DataView.new(buf)                =>  DataView
 *     DataView.new("buffer")           =>  DataView
 *
 *  Creates a new DataView instance. Both ArrayBuffer and data (String) constructors are supported.
 *
 * === Examples
 *     buf = ArrayBuffer.new(8)         =>  ArrayBuffer
 *     view = DataView.new(buf)         =>  DataView
 *
 *     view = DataView.new(buf, 2)      =>  DataView
 *     view.byte_offset                 => 2
 *     view.byte_length                 => 6
 *
 *     view = DataView.new(buf, 2, 4)   =>  DataView
 *     view.byte_offset                 => 2
 *     view.byte_length                 => 2
 *
 *     view = DataView.new("buffer")    =>  DataView
 *
*/
static VALUE rb_data_view_s_new(int argc, VALUE *argv, VALUE klass)
{
    VALUE data_view;
    unsigned long new_byte_length;
    rb_data_view_t *view = NULL;
    VALUE obj, byte_offset, byte_length, buffer;
    new_byte_length = 0;
    rb_scan_args(argc, argv, "12", &obj, &byte_offset, &byte_length);
    data_view = Data_Make_Struct(klass, rb_data_view_t, rb_mark_data_view, rb_free_data_view, view);
    view->byte_offset = 0;
    view->byte_length = 0;
    if (rb_class_of(obj) == rb_cArrayBuffer) { // ArrayBuffer constructor
        GetArrayBuffer(obj);
        view->byte_length = buf->length;
        view->buf = obj;
    } else if (rb_type(obj) == T_STRING){ // String constructor
        ArrayBufferEncode(obj);
        buffer = rb_alloc_array_buffer((unsigned long)RSTRING_LEN(obj), (void *)RSTRING_PTR(obj));
        GetArrayBuffer(buffer);
        view->byte_length = buf->length;
        view->buf = buffer;
    } else {
        rb_raise(rb_eTypeError, "DataView constructor %s not supported.", RSTRING_PTR(rb_obj_as_string(obj)));
    }
    if (!NIL_P(byte_offset)) {
        Check_Type(byte_offset, T_FIXNUM);
         view->byte_offset = FIX2ULONG(byte_offset);
         if (view->byte_offset >= view->byte_length)
            rb_raise(rb_eRangeError, "Byte offset out of range.");
    }
    if (!NIL_P(byte_length)) {
        Check_Type(byte_length, T_FIXNUM);
        new_byte_length = FIX2ULONG(byte_length);
        if (new_byte_length > view->byte_length)
            rb_raise(rb_eRangeError, "Byte length out of range.");
        if ((view->byte_offset + new_byte_length) > view->byte_length)
            rb_raise(rb_eRangeError, "Byte offset / length is not aligned.");
        view->byte_length = new_byte_length;
    }
    view->byte_length -= view->byte_offset;
    rb_obj_call_init(data_view, 0, NULL);
    return data_view;
}

/*
 *  call-seq:
 *     view.byte_length                =>  Fixnum
 *
 *  Returns the size of the underlying buffer managed by this DataView instance.
 *
 * === Examples
 *     view = DataView.new("buffer")   =>  DataView
 *     view.byte_length                =>  6
 *
*/
static VALUE rb_data_view_byte_length(VALUE obj)
{
    GetDataView(obj);
    return ULONG2NUM(view->byte_length);
}

/*
 *  call-seq:
 *     view.byte_offset                  =>  Fixnum
 *
 *  Returns the offset into the underlying buffer managed by this DataView instance.
 *
 * === Examples
 *     view = DataView.new("buffer")     =>  DataView
 *     view.byte_offset                  =>  0
 *
 *     view = DataView.new("buffer", 2)  =>  DataView
 *     view.byte_offset                  =>  2
 *
*/
static VALUE rb_data_view_byte_offset(VALUE obj)
{
    GetDataView(obj);
    return ULONG2NUM(view->byte_offset);
}

/*
 *  call-seq:
 *     view.buffer                     =>  String
 *
 *  Returns the underlying buffer managed by this DataView instance.
 *
 * === Examples
 *     view = DataView.new("buffer")   =>  DataView
 *     view.buffer                     =>  ArrayBuffer
 *
*/
static VALUE rb_data_view_buffer(VALUE obj)
{
    GetDataView(obj);
    return view->buf; 
}

/*
 *  call-seq:
 *     view.set_int8(2, 2)         =>  nil
 *
 *  Sets an int8 value at a given offset
 *
 * === Examples
 *     buf = ArrayBuffer.new(100)  => ArrayBuffer
 *     view = DataView.new(buf)    =>  DataView
 *     view.set_int8(2, 5)         =>  nil
 *     view.set_int8(3, -3)        =>  nil
 *
*/
static VALUE rb_data_view_set_int8(VALUE obj, VALUE offset_, VALUE item_)
{
    int argc;
    VALUE argv[3];
    argc = 3;
    argv[0] = offset_;
    argv[1] = item_;
    argv[2] = Qfalse;
    DataViewAset(obj);
    rb_type_array_set_int8(buf->buf, index, NUM2CHR(item), little_endian);
    return Qnil;
}

/*
 *  call-seq:
 *     view.get_int8(2)            =>  Fixnum
 *
 *  Gets an int8 value from a given offset
 *
 * === Examples
 *     buf = ArrayBuffer.new(100)  => ArrayBuffer
 *     view = DataView.new(buf)    =>  DataView
 *     view.set_int8(2, 5)         =>  nil
 *     view.get_int8(2)            =>  5
 *
*/
static VALUE rb_data_view_get_int8(VALUE obj, VALUE offset_)
{
    int argc;
    VALUE argv[2];
    argc = 1;
    argv[0] = offset_;
    argv[1] = Qfalse;
    DataViewAget(obj);
    return CHR2FIX(rb_type_array_get_int8(buf->buf, index, little_endian));
}

/*
 *  call-seq:
 *     view.set_uint8(2, 2)        =>  nil
 *
 *  Sets an unsigned int8 value at a given offset.
 *
 * === Examples
 *     buf = ArrayBuffer.new(100)  => ArrayBuffer
 *     view = DataView.new(buf)    =>  DataView
 *     view.set_uint8(2, 5)        =>  nil
 *
*/
static VALUE rb_data_view_set_uint8(VALUE obj, VALUE offset_, VALUE item_)
{
    int argc;
    VALUE argv[3];
    argc = 3;
    argv[0] = offset_;
    argv[1] = item_;
    argv[2] = Qfalse;
    DataViewAset(obj);
    rb_type_array_set_uint8(buf->buf, index, (unsigned char)NUM2CHR(item), little_endian);
    return Qnil;
}

/*
 *  call-seq:
 *     view.get_uint8(2)           =>  Fixnum
 *
 *  Gets an unsigned int8 value from a given offset.
 *
 * === Examples
 *     buf = ArrayBuffer.new(100)  => ArrayBuffer
 *     view = DataView.new(buf)    =>  DataView
 *     view.set_uint8(2, 5)        =>  nil
 *     view.get_uint8(2)           =>  5
 *
*/
static VALUE rb_data_view_get_uint8(VALUE obj, VALUE offset_)
{
    int argc;
    VALUE argv[2];
    argc = 1;
    argv[0] = offset_;
    argv[1] = Qfalse;
    DataViewAget(obj);
    return CHR2FIX(rb_type_array_get_uint8(buf->buf, index, little_endian));
}

/*
 *  call-seq:
 *     view.set_int16(2, 20)       =>  nil
 *
 *  Sets an int16 value at a given offset, using the provided endianness.
 *
 * === Examples
 *     buf = ArrayBuffer.new(100)  => ArrayBuffer
 *     view = DataView.new(buf)    =>  DataView
 *     view.set_int16(2, 20)       =>  nil
 *     view.set_int16(3, -2)       =>  nil
 *
*/
static VALUE rb_data_view_set_int16(int argc, VALUE *argv, VALUE obj)
{
    DataViewAset(obj);
    rb_type_array_set_int16(buf->buf, index, (short)NUM2INT(item), little_endian);
    return Qnil;
}

/*
 *  call-seq:
 *     view.get_int16(2)           =>  Fixnum
 *
 *  Gets an int16 value from a given offset, using the provided endianness.
 *
 * === Examples
 *     buf = ArrayBuffer.new(100)  => ArrayBuffer
 *     view = DataView.new(buf)    =>  DataView
 *     view.set_int16(2, 20)       =>  nil
 *     view.get_int16(2)           =>  20
 *
*/
static VALUE rb_data_view_get_int16(int argc, VALUE *argv, VALUE obj)
{
    DataViewAget(obj);
    return INT2FIX(rb_type_array_get_int16(buf->buf, index, little_endian));
}

/*
 *  call-seq:
 *     view.set_uint16(2, 20)      =>  nil
 *
 *  Sets an unsigned int16 value at a given offset, using the provided endianness.
 *
 * === Examples
 *     buf = ArrayBuffer.new(100)  => ArrayBuffer
 *     view = DataView.new(buf)    =>  DataView
 *     view.set_uint16(2, 20)      =>  nil
 *
*/
static VALUE rb_data_view_set_uint16(int argc, VALUE *argv, VALUE obj)
{
    DataViewAset(obj);
    rb_type_array_set_uint16(buf->buf, index, (unsigned short)NUM2INT(item), little_endian);
    return Qnil;
}

/*
 *  call-seq:
 *     view.get_uint16(2)          =>  Fixnum
 *
 *  Gets an unsigned int16 value from a given offset, using the provided endianness.
 *
 * === Examples
 *     buf = ArrayBuffer.new(100)  => ArrayBuffer
 *     view = DataView.new(buf)    =>  DataView
 *     view.set_uint16(2, 20)      =>  nil
 *     view.get_uint16(2)          =>  20
 *
*/
static VALUE rb_data_view_get_uint16(int argc, VALUE *argv, VALUE obj)
{
    DataViewAget(obj);
    return INT2FIX(rb_type_array_get_uint16(buf->buf, index, little_endian));
}

/*
 *  call-seq:
 *     view.set_int32(2, 658)      =>  nil
 *
 *  Sets an int32 value at a given offset, using the provided endianness.
 *
 * === Examples
 *     buf = ArrayBuffer.new(100)  => ArrayBuffer
 *     view = DataView.new(buf)    =>  DataView
 *     view.set_int32(2, 758)      =>  nil
 *     view.set_int32(3, -23)      =>  nil
 *
*/
static VALUE rb_data_view_set_int32(int argc, VALUE *argv, VALUE obj)
{
    DataViewAset(obj);
    rb_type_array_set_int32(buf->buf, index, NUM2INT(item), little_endian);
    return Qnil;
}

/*
 *  call-seq:
 *     view.get_int32(2)           =>  Fixnum
 *
 *  Gets an int32 value from a given offset, using the provided endianness.
 *
 * === Examples
 *     buf = ArrayBuffer.new(100)  => ArrayBuffer
 *     view = DataView.new(buf)    =>  DataView
 *     view.set_int32(2, 758)      =>  nil
 *     view.get_int32(2)           =>  758
 *
*/
static VALUE rb_data_view_get_int32(int argc, VALUE *argv, VALUE obj)
{
    DataViewAget(obj);
    return INT2FIX(rb_type_array_get_int32(buf->buf, index, little_endian));
}

/*
 *  call-seq:
 *     view.set_uint32(2, 20)      =>  nil
 *
 *  Sets an unsigned int32 value at a given offset, using the provided endianness.
 *
 * === Examples
 *     buf = ArrayBuffer.new(100)  => ArrayBuffer
 *     view = DataView.new(buf)    =>  DataView
 *     view.set_uint32(2, 758)     =>  nil
 *
*/
static VALUE rb_data_view_set_uint32(int argc, VALUE *argv, VALUE obj)
{
    DataViewAset(obj);
    rb_type_array_set_uint32(buf->buf, index, (unsigned int)NUM2INT(item), little_endian);
    return Qnil;
}

/*
 *  call-seq:
 *     view.get_uint32(2)          =>  Fixnum
 *
 *  Gets an unsigned int32 value from a given offset, using the provided endianness.
 *
 * === Examples
 *     buf = ArrayBuffer.new(100)  => ArrayBuffer
 *     view = DataView.new(buf)    =>  DataView
 *     view.set_uint32(2, 758)     =>  nil
 *     view.get_uint32(2)          =>  758
 *
*/
static VALUE rb_data_view_get_uint32(int argc, VALUE *argv, VALUE obj)
{
    DataViewAget(obj);
    return INT2FIX(rb_type_array_get_uint32(buf->buf, index, little_endian));
}

/*
 *  call-seq:
 *     view.set_float32(2, 0.775)   =>  nil
 *
 *  Sets a float32 value at a given offset, using the provided endianness.
 *
 * === Examples
 *     buf = ArrayBuffer.new(100)  => ArrayBuffer
 *     view = DataView.new(buf)    =>  DataView
 *     view.set_float32(2, 0.775)  =>  nil
 *
*/
static VALUE rb_data_view_set_float32(int argc, VALUE *argv, VALUE obj)
{
    float val;
    DataViewAset(obj);
    switch (TYPE(item)) {
      case T_FIXNUM:
          val = (float)FIX2LONG(item);
          break;
      case T_BIGNUM:
          val = (float)rb_big2dbl(item);
          break;
      case T_FLOAT:
          val = (float)RFLOAT_VALUE(item);
          break;
      default:
          rb_raise(rb_eTypeError, "Type arrays only support Fixnum, Bignum and Float instances");
    }
    rb_type_array_set_float32(buf->buf, index, val, little_endian);
    return Qnil;
}

/*
 *  call-seq:
 *     view.get_float32(2)          =>  Float
 *
 *  Gets a float32 value from a given offset, using the provided endianness.
 *
 * === Examples
 *     buf = ArrayBuffer.new(100)   =>  ArrayBuffer
 *     view = DataView.new(buf)     =>  DataView
 *     view.set_float32(2, 77.643)  =>  nil
 *     view.get_float32(2)          =>  758
 *
*/
static VALUE rb_data_view_get_float32(int argc, VALUE *argv, VALUE obj)
{
    DataViewAget(obj);
    return rb_float_new((double)rb_type_array_get_float32(buf->buf, index, little_endian));
}

/*
 *  call-seq:
 *     view.set_float64(2, 77.643)   =>  nil
 *
 *  Sets a float64 value at a given offset, using the provided endianness.
 *
 * === Examples
 *     buf = ArrayBuffer.new(100)    =>  ArrayBuffer
 *     view = DataView.new(buf)      =>  DataView
 *     view.set_float64(2, 77.643)   =>  nil
 *
*/
static VALUE rb_data_view_set_float64(int argc, VALUE *argv, VALUE obj)
{
    double val;
    DataViewAset(obj);
    switch (TYPE(item)) {
      case T_FIXNUM:
          val = (double)FIX2LONG(item);
          break;
      case T_BIGNUM:
          val = rb_big2dbl(item);
          break;
      case T_FLOAT:
          val = RFLOAT_VALUE(item);
          break;
      default:
          rb_raise(rb_eTypeError, "Type arrays only support Fixnum, Bignum and Float instances");
    }
    rb_type_array_set_float64(buf->buf, index, val, little_endian);
    return Qnil;
}

/*
 *  call-seq:
 *     view.get_float64(2)          =>  Float
 *
 *  Gets a float64 value from a given offset, using the provided endianness.
 *
 * === Examples
 *     buf = ArrayBuffer.new(100)   =>  ArrayBuffer
 *     view = DataView.new(buf)     =>  DataView
 *     view.set_float64(2, 77.643)  =>  nil
 *     view.get_float64(2)          =>  758
 *
*/
static VALUE rb_data_view_get_float64(int argc, VALUE *argv, VALUE obj)
{
    DataViewAget(obj);
    return rb_float_new(rb_type_array_get_float64(buf->buf, index, little_endian));
}

/*
 *  call-seq:
 *     view.to_s                        =>  String
 *
 *  Returns a String (binary) representation of the underlying buffer managed by this DataView instance.
 *
 * === Examples
 *     buf = ArrayBuffer.new("buffer")  =>  ArrayBuffer
 *     view = DataView.new(buf)         =>  DataView
 *     view.to_s                        =>  "buffer"
 *
*/
static VALUE rb_data_view_to_s(VALUE obj)
{
    GetDataView(obj);
    return rb_array_buffer_to_s(view->buf);
}

void _init_data_view()
{
    rb_cDataView = rb_define_class("DataView", rb_cObject);

    rb_define_singleton_method(rb_cDataView, "new", rb_data_view_s_new, -1);
    rb_define_method(rb_cDataView, "byte_length", rb_data_view_byte_length, 0);
    rb_define_method(rb_cDataView, "byte_offset", rb_data_view_byte_offset, 0);
    rb_define_method(rb_cDataView, "buffer", rb_data_view_buffer, 0);
    rb_define_method(rb_cDataView, "to_s", rb_data_view_to_s, 0);

    rb_define_method(rb_cDataView, "set_int8", rb_data_view_set_int8, 2);
    rb_define_method(rb_cDataView, "set_uint8", rb_data_view_set_uint8, 2);
    rb_define_method(rb_cDataView, "set_int16", rb_data_view_set_int16, -1);
    rb_define_method(rb_cDataView, "set_uint16", rb_data_view_set_uint16, -1);
    rb_define_method(rb_cDataView, "set_int32", rb_data_view_set_int32, -1);
    rb_define_method(rb_cDataView, "set_uint32", rb_data_view_set_uint32, -1);
    rb_define_method(rb_cDataView, "set_float32", rb_data_view_set_float32, -1);
    rb_define_method(rb_cDataView, "set_float64", rb_data_view_set_float64, -1);

    rb_define_method(rb_cDataView, "get_int8", rb_data_view_get_int8, 1);
    rb_define_method(rb_cDataView, "get_uint8", rb_data_view_get_uint8, 1);
    rb_define_method(rb_cDataView, "get_int16", rb_data_view_get_int16, -1);
    rb_define_method(rb_cDataView, "get_uint16", rb_data_view_get_uint16, -1);
    rb_define_method(rb_cDataView, "get_int32", rb_data_view_get_int32, -1);
    rb_define_method(rb_cDataView, "get_uint32", rb_data_view_get_uint32, -1);
    rb_define_method(rb_cDataView, "get_float32", rb_data_view_get_float32, -1);
    rb_define_method(rb_cDataView, "get_float64", rb_data_view_get_float64, -1);
}