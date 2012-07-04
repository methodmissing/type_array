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

static ID rb_type_array_intern_aget;
static ID rb_type_array_intern_aset;

static void rb_type_array_coerce_to_int8(rb_array_buffer_t *buf, long index, VALUE item)
{
     rb_type_array_set_int8(buf, index, (char)NUM2CHR(item), TYPE_ARRAY_IS_LITTLE_ENDIAN);
}

static VALUE rb_type_array_coerce_from_int8(rb_array_buffer_t *buf, long index)
{
     char val = rb_type_array_get_int8(buf, index, TYPE_ARRAY_IS_LITTLE_ENDIAN);
     return CHR2FIX(val);
}

static void rb_type_array_coerce_to_uint8(rb_array_buffer_t *buf, long index, VALUE item)
{
    rb_type_array_set_uint8(buf, index, (unsigned char)NUM2CHR(item), TYPE_ARRAY_IS_LITTLE_ENDIAN);
}

static VALUE rb_type_array_coerce_from_uint8(rb_array_buffer_t *buf, long index)
{
    unsigned char val = rb_type_array_get_uint8(buf, index, TYPE_ARRAY_IS_LITTLE_ENDIAN);
    return CHR2FIX(val);
}

static void rb_type_array_coerce_to_int16(rb_array_buffer_t *buf, long index, VALUE item)
{
    rb_type_array_set_int16(buf, index, (short)NUM2INT(item), TYPE_ARRAY_IS_LITTLE_ENDIAN);
}

static VALUE rb_type_array_coerce_from_int16(rb_array_buffer_t *buf, long index)
{
    short val = rb_type_array_get_int16(buf, index, TYPE_ARRAY_IS_LITTLE_ENDIAN);
    return INT2FIX(val);
}

static void rb_type_array_coerce_to_uint16(rb_array_buffer_t *buf, long index, VALUE item)
{
    rb_type_array_set_uint16(buf, index, (unsigned short)NUM2INT(item), TYPE_ARRAY_IS_LITTLE_ENDIAN);
}

static VALUE rb_type_array_coerce_from_uint16(rb_array_buffer_t *buf, long index)
{
    unsigned short val = rb_type_array_get_uint16(buf, index, TYPE_ARRAY_IS_LITTLE_ENDIAN);
    return INT2FIX(val);
}

static void rb_type_array_coerce_to_int32(rb_array_buffer_t *buf, long index, VALUE item)
{
    rb_type_array_set_int32(buf, index, NUM2INT(item), TYPE_ARRAY_IS_LITTLE_ENDIAN);
}

static VALUE rb_type_array_coerce_from_int32(rb_array_buffer_t *buf, long index)
{
    int val = rb_type_array_get_int32(buf, index, TYPE_ARRAY_IS_LITTLE_ENDIAN);
    return INT2FIX(val);
}

static void rb_type_array_coerce_to_uint32(rb_array_buffer_t *buf, long index, VALUE item)
{
    rb_type_array_set_uint32(buf, index, NUM2UINT(item), TYPE_ARRAY_IS_LITTLE_ENDIAN);
}

static VALUE rb_type_array_coerce_from_uint32(rb_array_buffer_t *buf, long index)
{
    unsigned int val = rb_type_array_get_uint32(buf, index, TYPE_ARRAY_IS_LITTLE_ENDIAN); 
    return UINT2NUM(val);
}

static void rb_type_array_coerce_to_float32(rb_array_buffer_t *buf, long index, VALUE item)
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

static VALUE rb_type_array_coerce_from_float32(rb_array_buffer_t *buf, long index)
{
    float val = rb_type_array_get_float32(buf, index, TYPE_ARRAY_IS_LITTLE_ENDIAN);
    return rb_float_new((double)val);
}

static void rb_type_array_coerce_to_float64(rb_array_buffer_t *buf, long index, VALUE item)
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

static VALUE rb_type_array_coerce_from_float64(rb_array_buffer_t *buf, long index)
{
    double val = rb_type_array_get_float64(buf, index, TYPE_ARRAY_IS_LITTLE_ENDIAN);
    return rb_float_new(val);
}

int rb_type_array_assert_alignment(unsigned long val, unsigned long bytes) {
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

    if (klass == rb_cInt8Array) {
        array->c_to_rb = rb_type_array_coerce_from_int8;
        array->rb_to_c = rb_type_array_coerce_to_int8;
    } else if (klass == rb_cUInt8Array) {
        array->c_to_rb = rb_type_array_coerce_from_uint8;
        array->rb_to_c = rb_type_array_coerce_to_uint8;
    } else if (klass == rb_cInt16Array) {
        array->c_to_rb = rb_type_array_coerce_from_int16;
        array->rb_to_c = rb_type_array_coerce_to_int16;
    } else if (klass == rb_cUInt16Array) {
        array->c_to_rb = rb_type_array_coerce_from_uint16;
        array->rb_to_c = rb_type_array_coerce_to_uint16;
    } else if (klass == rb_cInt32Array) {
        array->c_to_rb = rb_type_array_coerce_from_int32;
        array->rb_to_c = rb_type_array_coerce_to_int32;
    } else if (klass == rb_cUInt32Array) {
        array->c_to_rb = rb_type_array_coerce_from_uint32;
        array->rb_to_c = rb_type_array_coerce_to_uint32;
    } else if (klass == rb_cFloat32Array) {
        array->c_to_rb = rb_type_array_coerce_from_float32;
        array->rb_to_c = rb_type_array_coerce_to_float32;
    } else if (klass == rb_cFloat64Array) {
        array->c_to_rb = rb_type_array_coerce_from_float64;
        array->rb_to_c = rb_type_array_coerce_to_float64;
    }

    if (FIXNUM_P(obj)) { // Length constructor
        array->length = FIX2ULONG(obj);
        array->byte_length = (array->length * array->size);
        array->buf = rb_alloc_array_buffer(array->byte_length, NULL);
    } else if (rb_type(obj) == T_STRING) { // String constructor
        array->byte_length = (unsigned long)RSTRING_LEN(obj);
        array->length = (array->byte_length / array->size);
        ArrayBufferEncode(obj);
        array->buf = rb_alloc_array_buffer(array->byte_length, (void *)RSTRING_PTR(obj));
    } else if (rb_class_of(obj) == rb_cArrayBuffer) { // ArrayBuffer constructor
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
    } else if (rb_obj_is_kind_of(obj, rb_cTypeArray) == Qtrue) { // TypeArray constructor
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

static VALUE rb_type_array_to_s(VALUE obj)
{
    GetTypeArray(obj);
    return rb_array_buffer_to_s(ary->buf); 
}

static VALUE rb_type_array_byte_offset(VALUE obj)
{
    GetTypeArray(obj);
    return ULONG2NUM(ary->byte_offset);
}

inline long rb_type_array_aset0(VALUE idx, rb_type_array_t *ary, VALUE item)
{
    long index;
    Check_Type(idx, T_FIXNUM);
    index = FIX2LONG(idx) * ary->size;
    if (index < 0) rb_raise(rb_eRangeError, "Offset may not be negative.");
    if (!rb_type_array_assert_alignment(index, ary->size)) rb_raise(rb_eRangeError, "Byte offset is not aligned.");
    if ((unsigned long)index > ary->byte_length) rb_raise(rb_eRangeError, "Offset out of range.");
    if (ary->size > (ary->byte_length - (unsigned long)index)) rb_raise(rb_eRangeError, "Offset/length out of range.");
    switch (TYPE(item)) {
    case T_FIXNUM:
    case T_BIGNUM:
    case T_FLOAT:
        break;
    default:
        rb_raise(rb_eTypeError, "Type arrays only support Fixnum, Bignum and Float instances");
    }
    return index;
}

inline long rb_type_array_aget0(VALUE idx, rb_type_array_t *ary)
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

static VALUE rb_type_array_aset(VALUE obj, VALUE idx, VALUE item)
{
    GetTypeArray(obj);
    GetArrayBuffer(ary->buf);
    long index = rb_type_array_aset0(idx, ary, item);
    ary->rb_to_c(buf->buf, index, item);
    return Qnil;
}

static VALUE rb_type_array_aget(VALUE obj, VALUE idx)
{
    GetTypeArray(obj);
    GetArrayBuffer(ary->buf);
    long index = rb_type_array_aget0(idx, ary);
    return ary->c_to_rb(buf->buf, index);
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