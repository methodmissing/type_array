#include <type_array_ext.h>

/*
 * :nodoc:
 *  Provides a typed interface to a given ArrayBuffer instance. Values will only be coerced according to the type of
 *  array. Currently coercing to and from Ruby objects are handled through callbacks via function pointers. This may
 *  change in the short term.
 *
*/

VALUE rb_cTypeArray;

VALUE rb_cInt8Array;
VALUE rb_cUInt8Array;
VALUE rb_cInt16Array;
VALUE rb_cUInt16Array;
VALUE rb_cInt32Array;
VALUE rb_cUInt32Array;
VALUE rb_cFloat32Array;
VALUE rb_cFloat64Array;

static ID rb_type_array_intern_aget;
static ID rb_type_array_intern_aset;

/*
 * :nodoc:
 *  Coerces a Ruby object to an int8 at a given offset, according to host endianness.
 *
*/
DefineTypeArraySetter(int8, (signed char)NUM2CHR(item));

/*
 * :nodoc:
 *  Coerces an int8 at a given offset to a Ruby object, according to host endianness.
 *
*/
DefineTypeArrayGetter(int8, signed char, TACHR2FIX(val));

/*
 * :nodoc:
 *  Coerces a Ruby object to an unsigned int8 at a given offset, according to host endianness.
 *
*/
DefineTypeArraySetter(uint8, (unsigned char)NUM2CHR(item));

/*
 * :nodoc:
 *  Coerces an unsigned int8 at a given offset to a Ruby object, according to host endianness.
 *
*/
DefineTypeArrayGetter(uint8, unsigned char, TACHR2FIX(val));

/*
 * :nodoc:
 *  Coerces a Ruby object to an int16 at a given offset, according to host endianness.
 *
*/
DefineTypeArraySetter(int16, (short)NUM2INT(item));

/*
 * :nodoc:
 *  Coerces an int16 at a given offset to a Ruby object, according to host endianness.
 *
*/
DefineTypeArrayGetter(int16, short, INT2FIX(val));

/*
 * :nodoc:
 *  Coerces a Ruby object to an unsigned int16 at a given offset, according to host endianness.
 *
*/
DefineTypeArraySetter(uint16, (unsigned short)NUM2INT(item));

/*
 * :nodoc:
 *  Coerces an unsigned int16 at a given offset to a Ruby object, according to host endianness.
 *
*/
DefineTypeArrayGetter(uint16, unsigned short, INT2FIX(val));

/*
 * :nodoc:
 *  Coerces a Ruby object to an int32 at a given offset, according to host endianness.
 *
*/
DefineTypeArraySetter(int32, NUM2INT(item));

/*
 * :nodoc:
 *  Coerces an int32 at a given offset to a Ruby object, according to host endianness.
 *
*/
DefineTypeArrayGetter(int32, int, INT2FIX(val));

/*
 * :nodoc:
 *  Coerces a Ruby object to an unsigned int32 at a given offset, according to host endianness.
 *
*/
DefineTypeArraySetter(uint32, NUM2UINT(item));

/*
 * :nodoc:
 *  Coerces an unsigned int32 at a given offset to a Ruby object, according to host endianness.
 *
*/
DefineTypeArrayGetter(uint32, unsigned int, UINT2NUM(val));

/*
 * :nodoc:
 *  Coerces a Ruby object to a float32 (float) at a given offset, according to host endianness.
 *
*/
static void rb_type_array_aset_float32(rb_array_buffer_t *buf, long index, VALUE item)
{
    float val;
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
    rb_type_array_set_float32(buf, index, val, TYPE_ARRAY_IS_LITTLE_ENDIAN);
}

/*
 * :nodoc:
 *  Coerces a float32 (float) at a given offset to a Ruby object, according to host endianness.
 *
*/
DefineTypeArrayGetter(float32, float, rb_float_new((double)val));

/*
 * :nodoc:
 *  Coerces a Ruby object to a float64 (double) at a given offset, according to host endianness.
 *
*/
static void rb_type_array_aset_float64(rb_array_buffer_t *buf, long index, VALUE item)
{
    double val;
    switch (TYPE(item)) {
      case T_FIXNUM:
          val = FIX2LONG(item);
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
    rb_type_array_set_float64(buf, index, val, TYPE_ARRAY_IS_LITTLE_ENDIAN);
}

/*
 * :nodoc:
 *  Coerces a float64 (double) at a given offset to a Ruby object, according to host endianness.
 *
*/
DefineTypeArrayGetter(float64, double, rb_float_new(val));

/*
 * :nodoc:
 *  Asserts type alignment.
 *
*/
inline int rb_type_array_assert_alignment(unsigned long val, unsigned long bytes) {
    return (val & (bytes - 1)) == 0 ? 1 : 0;
}

/*
 * :nodoc:
 *  Swizzles byte order.
 *
*/
inline void rb_type_array_swizzle(char* buf, unsigned long len) {
    unsigned long i;
    for (i = 0; i < len / 2; ++i) {
      char t = buf[i];
      buf[i] = buf[len - i - 1];
      buf[len - i - 1] = t;
    }
}

/*
 * :nodoc:
 *  Validates offset boundaries.
 *
*/
static inline long rb_type_array_assert_offset(rb_type_array_t *ary, VALUE idx)
{
    long index;
    Check_Type(idx, T_FIXNUM);
    index = FIX2LONG(idx) * ary->size;
    if (index < 0) rb_raise(rb_eRangeError, "Offset may not be negative.");
    if (!rb_type_array_assert_alignment(index, ary->size)) rb_raise(rb_eRangeError, "Byte offset is not aligned.");
    if ((unsigned long)index > ary->byte_length) rb_raise(rb_eRangeError, "Offset out of range.");
    if (ary->size > (ary->byte_length - (unsigned long)index)) rb_raise(rb_eRangeError, "Offset/length out of range.");
    return index;
}

/*
 * :nodoc:
 *  GC mark callback
 *
*/
static void rb_mark_type_array(void *ptr)
{
    rb_type_array_t *ary = (rb_type_array_t *)ptr;
    if (ary) rb_gc_mark(ary->buf);
}

/*
 * :nodoc:
 *  GC free callback
 *
*/
static void rb_free_type_array(void *ptr)
{
    rb_type_array_t *ary = (rb_type_array_t *)ptr;
#ifdef TYPE_ARRAY_DEBUG
    printf(">> rb_free_type_array %p\n", ptr);
#endif
    if (ary) {
        xfree(ary);
        ary = NULL;
    }
#ifdef TYPE_ARRAY_DEBUG
    printf("<< rb_free_type_array %p\n", ptr);
#endif
}

/*
 *  call-seq:
 *     Int32Array.new(100)                =>  Int32Array
 *     Int32Array.new("01234567")         =>  Int32Array
 *     Int32Array.new(buf, 20)            =>  Int32Array
 *     Int32Array.new(buf, 0, 20)         =>  Int32Array
 *     Int32Array.new(buf, 20, 20)        =>  Int32Array
 *
 *  Creates a new TypeArray instance. ArrayBuffer, data (String) and length constructors are supported.
 *
 * === Examples
 *     buf = ArrayBuffer.new(100)         =>  ArrayBuffer
 *
 *     ary = Int32Array.new(buf, 20)      =>  Int32Array
 *     ary.length                         =>  20
 *     ary.byte_length                    =>  80
 *     ary.byte_offset                    =>  20
 *
 *     ary = Int32Array.new(buf, 0, 20)   =>  Int32Array
 *     ary.length                         =>  20
 *     ary.byte_length                    =>  80
 *     ary.byte_offset                    =>  0
 *
 *     ary = Int32Array.new(buf, 20, 20)  =>  Int32Array
 *     ary.length                         =>  20
 *     ary.byte_length                    =>  80
 *     ary.byte_offset                    =>  20
 *
 *     ary = Int32Array.new("01234567")   =>  Int32Array
 *     ary.byte_length                    =>  8
 *     ary.to_s                           =>  "01234567"
 *
 *     ary = Int32Array.new(100)          =>  Int32Array
 *     ary.length                         =>  100
 *     ary.byte_length                    =>  400
*/
static VALUE rb_type_array_s_new(int argc, VALUE *argv, VALUE klass)
{
    VALUE type_array;
    VALUE obj, byte_offset, length;
    rb_type_array_t *array = NULL;
    unsigned long buffer_length, offset;
    if (klass == rb_cTypeArray) rb_raise(rb_eTypeError, "TypeArray cannot be instantiated directly.");
    rb_scan_args(argc, argv, "12", &obj, &byte_offset, &length);
    type_array = Data_Make_Struct(klass, rb_type_array_t, rb_mark_type_array, rb_free_type_array, array);
    array->size = FIX2ULONG(rb_const_get(klass, rb_intern("BYTES_PER_ELEMENT")));
    array->byte_offset = 0;
    array->length = 0;

    if (klass == rb_cInt8Array) {
        array->aref_fn = rb_type_array_aref_int8;
        array->aset_fn = rb_type_array_aset_int8;
    } else if (klass == rb_cUInt8Array) {
        array->aref_fn = rb_type_array_aref_uint8;
        array->aset_fn = rb_type_array_aset_uint8;
    } else if (klass == rb_cInt16Array) {
        array->aref_fn = rb_type_array_aref_int16;
        array->aset_fn = rb_type_array_aset_int16;
    } else if (klass == rb_cUInt16Array) {
        array->aref_fn = rb_type_array_aref_uint16;
        array->aset_fn = rb_type_array_aset_uint16;
    } else if (klass == rb_cInt32Array) {
        array->aref_fn = rb_type_array_aref_int32;
        array->aset_fn = rb_type_array_aset_int32;
    } else if (klass == rb_cUInt32Array) {
        array->aref_fn = rb_type_array_aref_uint32;
        array->aset_fn = rb_type_array_aset_uint32;
    } else if (klass == rb_cFloat32Array) {
        array->aref_fn = rb_type_array_aref_float32;
        array->aset_fn = rb_type_array_aset_float32;
    } else if (klass == rb_cFloat64Array) {
        array->aref_fn = rb_type_array_aref_float64;
        array->aset_fn = rb_type_array_aset_float64;
    }

    if (FIXNUM_P(obj)) {
        array->length = FIX2ULONG(obj);
        array->byte_length = (array->length * array->size);
        array->buf = rb_alloc_array_buffer(array->byte_length, NULL);
    } else if (rb_type(obj) == T_STRING) {
        array->byte_length = (unsigned long)RSTRING_LEN(obj);
        array->length = (array->byte_length / array->size);
        ArrayBufferEncode(obj);
        array->buf = rb_alloc_array_buffer(array->byte_length, (void *)RSTRING_PTR(obj));
    } else if (rb_class_of(obj) == rb_cArrayBuffer) {
        GetArrayBuffer(obj);
        if (!NIL_P(byte_offset)) {
            Check_Type(byte_offset, T_FIXNUM);
            array->byte_offset = FIX2ULONG(byte_offset);
            if (!rb_type_array_assert_alignment(array->byte_offset, array->size))
                rb_raise(rb_eRangeError, "Byte offset is not aligned.");
        }
        buffer_length = buf->length;
        if (!NIL_P(length)) {
            Check_Type(length, T_FIXNUM);
            array->length = FIX2ULONG(length);
            array->byte_length = array->length * array->size;
        } else {
            array->byte_length = buffer_length - array->byte_offset;
        }
        if ((array->byte_offset + array->byte_length) > buffer_length)
            rb_raise(rb_eRangeError, "Byte offset / length is not aligned.");
        if (array->length == 0) array->length = array->byte_length / array->size;
        if (array->byte_offset > buffer_length || array->byte_offset + array->length > buffer_length ||
             array->byte_offset + array->length * array->size > buffer_length) {
             rb_raise(rb_eRangeError, "Length is out of range.");
        }
        array->buf = obj;
    } else if (rb_obj_is_kind_of(obj, rb_cTypeArray) == Qtrue) {
        GetTypeArray(obj);
        array->length = ary->length;
        array->byte_length = (array->size * array->length);
        array->buf = rb_alloc_array_buffer(array->byte_length, NULL);
        array->byte_offset = 0;
        for (offset = 0; offset < array->length; ++offset) {
          VALUE offs = INT2FIX(offset);
          VALUE val = rb_funcall(obj, rb_type_array_intern_aget, 1, offs);
          rb_funcall(type_array, rb_type_array_intern_aset, 2, offs, val);
        }
    } else {
        rb_raise(rb_eTypeError, "TypeArray constructor %s not supported.", RSTRING_PTR(rb_obj_as_string(obj)));
    }
    rb_obj_call_init(type_array, 0, NULL);
    return type_array;
}

/*
 *  call-seq:
 *     ary.byte_length                   =>  Fixnum
 *
 *  Returns the size of the underlying buffer managed by this TypeArray instance.
 *
 * === Examples
 *     ary = Int32Array.new("01234567")  =>  Int32Array
 *     ary.byte_length                   =>  8
 *
*/
static VALUE rb_type_array_byte_length(VALUE obj)
{
    GetTypeArray(obj);
    return ULONG2NUM(ary->byte_length);
}

/*
 *  call-seq:
 *     ary.length                        =>  Fixnum
 *
 *  Returns the max number of elements this typed array instance can accommodate.
 *
 * === Examples
 *     ary = Int32Array.new("01234567")  =>  Int32Array
 *     ary.byte_length                   =>  8
 *     ary.length                        =>  2
 *
*/
static VALUE rb_type_array_length(VALUE obj)
{
    GetTypeArray(obj);
    return ULONG2NUM(ary->length);
}

/*
 *  call-seq:
 *     ary.buffer                      =>  String
 *
 *  Returns the underlying buffer managed by this ArrayBuffer instance.
 *
 * === Examples
 *     ary = Int32Array.new("buffer")  =>  DataView
 *     ary.buffer                      =>  ArrayBuffer
 *
*/
static VALUE rb_type_array_buffer(VALUE obj)
{
    GetTypeArray(obj);
    return ary->buf; 
}

/*
 *  call-seq:
 *     art.to_s                         =>  String
 *
 *  Returns a String (binary) representation of the underlying buffer managed by this TypeArray instance.
 *
 * === Examples
 *     buf = ArrayBuffer.new("buffer")  =>  ArrayBuffer
 *     ary = Int32Array.new(buf)        =>  Int32Array
 *     ary.to_s                         =>  "buffer"
 *
*/
static VALUE rb_type_array_to_s(VALUE obj)
{
    GetTypeArray(obj);
    return rb_array_buffer_to_s(ary->buf); 
}

/*
 *  call-seq:
 *     ary.byte_offset                      =>  Fixnum
 *
 *  Returns the offset into the underlying buffer managed by this TypeArray instance.
 *
 * === Examples
 *     ary = Int32Array.new("01234567")     =>  Int32Array
 *     ary.byte_offset                      =>  0
 *
 *     ary = Int32Array.new("01234567", 2)  =>  Int32Array
 *     ary.byte_offset                      =>  2
 *
*/
static VALUE rb_type_array_byte_offset(VALUE obj)
{
    GetTypeArray(obj);
    return ULONG2NUM(ary->byte_offset);
}

/*
 *  call-seq:
 *     ary[1] = 20                       =>  nil
 *
 *  Sets a value at a given offset, with coercion dependent on the array type of this instance.
 *
 * === Examples
 *     ary = Int32Array.new("01234567")  =>  Int32Array
 *     ary[1] = 23                       =>  nil
 *
*/
static VALUE rb_type_array_aset(VALUE obj, VALUE idx, VALUE item)
{
    GetTypeArray(obj);
    GetArrayBuffer(ary->buf);
    long index = rb_type_array_assert_offset(ary, idx);
    switch (TYPE(item)) {
    case T_FIXNUM:
    case T_BIGNUM:
    case T_FLOAT:
        break;
    default:
        rb_raise(rb_eTypeError, "Type arrays only support Fixnum, Bignum and Float instances");
    }
    ary->aset_fn(buf->buf, index, item);
    return Qnil;
}

/*
 *  call-seq:
 *     ary[1]                            =>  Fixnum, Bignum or Float
 *
 *  Gets a value at a given offset, with coercion dependent on the array type of this instance.
 *
 * === Examples
 *     ary = Int32Array.new("01234567")  =>  Int32Array
 *     ary[1] = 23                       =>  nil
 *     ary[1]                            =>  23
 *
*/
static VALUE rb_type_array_aget(VALUE obj, VALUE idx)
{
    GetTypeArray(obj);
    GetArrayBuffer(ary->buf);
    long index = rb_type_array_assert_offset(ary, idx);
    return ary->aref_fn(buf->buf, index);
}

void _init_type_array()
{
    rb_cTypeArray = rb_define_class("TypeArray", rb_cObject);

    rb_type_array_intern_aget = rb_intern("[]");
    rb_type_array_intern_aset = rb_intern("[]=");

    rb_cInt8Array = rb_define_class("Int8Array", rb_cTypeArray);
    rb_cUInt8Array = rb_define_class("UInt8Array", rb_cTypeArray);
    rb_cInt16Array = rb_define_class("Int16Array", rb_cTypeArray);
    rb_cUInt16Array = rb_define_class("UInt16Array", rb_cTypeArray);
    rb_cInt32Array = rb_define_class("Int32Array", rb_cTypeArray);
    rb_cUInt32Array = rb_define_class("UInt32Array", rb_cTypeArray);
    rb_cFloat32Array = rb_define_class("Float32Array", rb_cTypeArray);
    rb_cFloat64Array = rb_define_class("Float64Array", rb_cTypeArray);

    rb_define_const(rb_cInt8Array, "BYTES_PER_ELEMENT", ULONG2NUM(sizeof(signed char)));
    rb_define_const(rb_cUInt8Array, "BYTES_PER_ELEMENT", ULONG2NUM(sizeof(unsigned char)));
    rb_define_const(rb_cInt16Array, "BYTES_PER_ELEMENT", ULONG2NUM(sizeof(short)));
    rb_define_const(rb_cUInt16Array, "BYTES_PER_ELEMENT", ULONG2NUM(sizeof(unsigned short)));
    rb_define_const(rb_cInt32Array, "BYTES_PER_ELEMENT", ULONG2NUM(sizeof(int)));
    rb_define_const(rb_cUInt32Array, "BYTES_PER_ELEMENT", ULONG2NUM(sizeof(unsigned int)));
    rb_define_const(rb_cFloat32Array, "BYTES_PER_ELEMENT", ULONG2NUM(sizeof(float)));
    rb_define_const(rb_cFloat64Array, "BYTES_PER_ELEMENT", ULONG2NUM(sizeof(double)));

    rb_define_singleton_method(rb_cTypeArray, "new", rb_type_array_s_new, -1);

    rb_define_method(rb_cTypeArray, "byte_length", rb_type_array_byte_length, 0);
    rb_define_method(rb_cTypeArray, "length", rb_type_array_length, 0);
    rb_define_method(rb_cTypeArray, "buffer", rb_type_array_buffer, 0);
    rb_define_method(rb_cTypeArray, "byte_offset", rb_type_array_byte_offset, 0);
    rb_define_method(rb_cTypeArray, "to_s", rb_type_array_to_s, 0);
    rb_define_method(rb_cTypeArray, "[]=", rb_type_array_aset, 2);
    rb_define_method(rb_cTypeArray, "[]", rb_type_array_aget, 1);
}