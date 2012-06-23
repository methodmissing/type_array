#include <type_array_ext.h>

VALUE rb_cTypeArray;

VALUE rb_cInt8Array;
VALUE rb_cUInt8Array;
VALUE rb_cInt16Array;
VALUE rb_cUInt16Array;
VALUE rb_cInt32Array;
VALUE rb_cUInt32Array;
VALUE rb_cFloat32Array;
VALUE rb_cFloat64Array;

int rb_type_array_assert_alignment(unsigned int val, unsigned int bytes) {
  return (val & (bytes - 1)) == 0 ? 1 : 0;
}

void rb_type_array_swizzle(char* buf, unsigned long len) {
  unsigned long i;
  for (i = 0; i < len / 2; ++i) {
    char t = buf[i];
    buf[i] = buf[len - i - 1];
    buf[len - i - 1] = t;
  }
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
    if (ary) {
        xfree(ary);
        ary = NULL;
    }
}

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
    if (FIXNUM_P(obj)) { // Length constructor
        array->length = FIX2ULONG(obj);
        array->byte_length = (array->length * array->size);
        array->buf = rb_alloc_array_buffer(array->byte_length);
    } else if (rb_class_of(obj) == rb_cArrayBuffer) { // ArrayBuffer constructor
        GetArrayBuffer(obj);
        if (!NIL_P(byte_offset)) {
            Check_Type(byte_offset, T_FIXNUM);
            array->byte_offset = FIX2ULONG(byte_offset);
            if (!rb_type_array_assert_alignment(array->byte_offset, array->size)) rb_raise(rb_eRangeError, "Byte offset is not aligned.");
        }
        buffer_length = buf->length;
        if (!NIL_P(length)) {
            Check_Type(length, T_FIXNUM);
            array->length = FIX2ULONG(length);
            array->byte_length = array->length * array->size;
        } else {
            array->byte_length = buffer_length - array->byte_offset;
        }
        if ((array->byte_offset + array->byte_length) > buffer_length) {
            xfree(array);
            rb_raise(rb_eRangeError, "Byte offset / length is not aligned.");
        }
        if (array->length == 0) array->length = array->byte_length / array->size;
        if (array->byte_offset > buffer_length || array->byte_offset + array->length > buffer_length ||
             array->byte_offset + array->length * array->size > buffer_length) {
             xfree(array);
             rb_raise(rb_eRangeError, "Length is out of range.");
        }
        array->buf = obj;
    } else if (rb_obj_is_kind_of(obj, rb_cTypeArray) == Qtrue) { // TypeArray constructor
        GetTypeArray(obj);
        array->length = ary->length;
        array->byte_length = (array->size * array->length);
        array->buf = rb_alloc_array_buffer(array->byte_length);
        for (offset = 0; offset < array->length; ++offset) {
           
        }
    } else {
        xfree(array);
        rb_raise(rb_eTypeError, "TypeArray constructor %s not supported.", RSTRING_PTR(rb_obj_as_string(obj)));
    }
    rb_obj_call_init(type_array, 0, NULL);
    return type_array;
}

static VALUE rb_type_array_byte_length(VALUE obj)
{
    GetTypeArray(obj);
    return ULONG2NUM(ary->byte_length);
}

static VALUE rb_type_array_length(VALUE obj)
{
    GetTypeArray(obj);
    return ULONG2NUM(ary->length);
}

static VALUE rb_type_array_buffer(VALUE obj)
{
    GetTypeArray(obj);
    return ary->buf; 
}

static VALUE rb_type_array_byte_offset(VALUE obj)
{
    GetTypeArray(obj);
    return ULONG2NUM(ary->byte_offset);
}

static VALUE rb_int8_array_aset(VALUE obj, VALUE idx, VALUE item)
{
    TypeArrayAset(obj, idx, item);
    rb_type_array_set_int8(buf->buf, index, NUM2CHR(item), TYPE_ARRAY_IS_LITTLE_ENDIAN);
    return Qnil;
}

static VALUE rb_int8_array_aget(VALUE obj, VALUE idx)
{
    TypeArrayAget(obj, idx);
    return CHR2FIX(rb_type_array_get_int8(buf->buf, index, TYPE_ARRAY_IS_LITTLE_ENDIAN));
}

static VALUE rb_uint8_array_aset(VALUE obj, VALUE idx, VALUE item)
{
    TypeArrayAset(obj, idx, item);
    rb_type_array_set_uint8(buf->buf, index, (unsigned char)NUM2CHR(item), TYPE_ARRAY_IS_LITTLE_ENDIAN);
    return Qnil;
}

static VALUE rb_uint8_array_aget(VALUE obj, VALUE idx)
{
    TypeArrayAget(obj, idx);
    return CHR2FIX(rb_type_array_get_uint8(buf->buf, index, TYPE_ARRAY_IS_LITTLE_ENDIAN));
}

static VALUE rb_int16_array_aset(VALUE obj, VALUE idx, VALUE item)
{
    TypeArrayAset(obj, idx, item);
    rb_type_array_set_int16(buf->buf, index, (short)NUM2INT(item), TYPE_ARRAY_IS_LITTLE_ENDIAN);
    return Qnil;
}

static VALUE rb_int16_array_aget(VALUE obj, VALUE idx)
{
    TypeArrayAget(obj, idx);
    return INT2FIX(rb_type_array_get_int16(buf->buf, index, TYPE_ARRAY_IS_LITTLE_ENDIAN));
}

static VALUE rb_uint16_array_aset(VALUE obj, VALUE idx, VALUE item)
{
    TypeArrayAset(obj, idx, item);
    rb_type_array_set_uint16(buf->buf, index, (unsigned short)NUM2INT(item), TYPE_ARRAY_IS_LITTLE_ENDIAN);
    return Qnil;
}

static VALUE rb_uint16_array_aget(VALUE obj, VALUE idx)
{
    TypeArrayAget(obj, idx);
    return INT2FIX(rb_type_array_get_uint16(buf->buf, index, TYPE_ARRAY_IS_LITTLE_ENDIAN));
}

static VALUE rb_int32_array_aset(VALUE obj, VALUE idx, VALUE item)
{
    TypeArrayAset(obj, idx, item);
    rb_type_array_set_int32(buf->buf, index, NUM2INT(item), TYPE_ARRAY_IS_LITTLE_ENDIAN);
    return Qnil;
}

static VALUE rb_int32_array_aget(VALUE obj, VALUE idx)
{
    TypeArrayAget(obj, idx);
    return INT2FIX(rb_type_array_get_int32(buf->buf, index, TYPE_ARRAY_IS_LITTLE_ENDIAN));
}

static VALUE rb_uint32_array_aset(VALUE obj, VALUE idx, VALUE item)
{
    TypeArrayAset(obj, idx, item);
    rb_type_array_set_uint32(buf->buf, index, NUM2UINT(item), TYPE_ARRAY_IS_LITTLE_ENDIAN);
    return Qnil;
}

static VALUE rb_uint32_array_aget(VALUE obj, VALUE idx)
{
    TypeArrayAget(obj, idx);
    return UINT2NUM(rb_type_array_get_uint32(buf->buf, index, TYPE_ARRAY_IS_LITTLE_ENDIAN));
}

static VALUE rb_float32_array_aset(VALUE obj, VALUE idx, VALUE item)
{
    float val;
    TypeArrayAset(obj, idx, item);
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
    rb_type_array_set_float32(buf->buf, index, val, TYPE_ARRAY_IS_LITTLE_ENDIAN);
    return Qnil;
}

static VALUE rb_float32_array_aget(VALUE obj, VALUE idx)
{
    TypeArrayAget(obj, idx);
    return rb_float_new((double)rb_type_array_get_float32(buf->buf, index, TYPE_ARRAY_IS_LITTLE_ENDIAN));
}

static VALUE rb_float64_array_aset(VALUE obj, VALUE idx, VALUE item)
{
    double val;
    TypeArrayAset(obj, idx, item);
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
    rb_type_array_set_float64(buf->buf, index, val, TYPE_ARRAY_IS_LITTLE_ENDIAN);
    return Qnil;
}

static VALUE rb_float64_array_aget(VALUE obj, VALUE idx)
{
    TypeArrayAget(obj, idx);
    return rb_float_new(rb_type_array_get_float64(buf->buf, index, TYPE_ARRAY_IS_LITTLE_ENDIAN));
}

void _init_type_array()
{
    rb_cTypeArray = rb_define_class("TypeArray", rb_cObject);

    rb_define_method(rb_cTypeArray, "byte_length", rb_type_array_byte_length, 0);
    rb_define_method(rb_cTypeArray, "length", rb_type_array_length, 0);
    rb_define_method(rb_cTypeArray, "buffer", rb_type_array_buffer, 0);
    rb_define_method(rb_cTypeArray, "byte_offset", rb_type_array_byte_offset, 0);

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

    rb_define_method(rb_cInt8Array, "[]=", rb_int8_array_aset, 2);
    rb_define_method(rb_cInt8Array, "[]", rb_int8_array_aget, 1);
    rb_define_method(rb_cUInt8Array, "[]=", rb_uint8_array_aset, 2);
    rb_define_method(rb_cUInt8Array, "[]", rb_uint8_array_aget, 1);
    rb_define_method(rb_cInt16Array, "[]=", rb_int16_array_aset, 2);
    rb_define_method(rb_cInt16Array, "[]", rb_int16_array_aget, 1);
    rb_define_method(rb_cUInt16Array, "[]=", rb_uint16_array_aset, 2);
    rb_define_method(rb_cUInt16Array, "[]", rb_uint16_array_aget, 1);
    rb_define_method(rb_cInt32Array, "[]=", rb_int32_array_aset, 2);
    rb_define_method(rb_cInt32Array, "[]", rb_int32_array_aget, 1);
    rb_define_method(rb_cUInt32Array, "[]=", rb_uint32_array_aset, 2);
    rb_define_method(rb_cUInt32Array, "[]", rb_uint32_array_aget, 1);
    rb_define_method(rb_cFloat32Array, "[]=", rb_float32_array_aset, 2);
    rb_define_method(rb_cFloat32Array, "[]", rb_float32_array_aget, 1);
    rb_define_method(rb_cFloat64Array, "[]=", rb_float64_array_aset, 2);
    rb_define_method(rb_cFloat64Array, "[]", rb_float64_array_aget, 1);
}