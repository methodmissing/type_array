#include <type_array_ext.h>

VALUE rb_cDataView;

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
    if (view) xfree(view);
}

static VALUE rb_data_view_s_new(int argc, VALUE *argv, VALUE klass)
{
    VALUE data_view;
    rb_data_view_t *view = NULL;
    VALUE obj, byte_offset, byte_length;
    rb_scan_args(argc, argv, "12", &obj, &byte_offset, &byte_length);
    data_view = Data_Make_Struct(klass, rb_data_view_t, rb_mark_data_view, rb_free_data_view, view);
    view->byte_offset = 0;
    view->byte_length = 0;
    if (rb_class_of(obj) == rb_cArrayBuffer) { // ArrayBuffer constructor
        GetArrayBuffer(obj);
        view->byte_length = buf->length;
        if (!NIL_P(byte_length)) {
            Check_Type(byte_length, T_FIXNUM);
            view->byte_length = FIX2ULONG(byte_length);
            if (view->byte_length < 0) rb_raise(rb_eRangeError, "Length out of range.");
        }
        if (!NIL_P(byte_offset)) {
            Check_Type(byte_offset, T_FIXNUM);
            view->byte_offset = FIX2ULONG(byte_offset);
            if (view->byte_offset < 0) rb_raise(rb_eRangeError, "Byte offset out of range.");
            if (view->byte_offset % view->byte_length != 0) rb_raise(rb_eRangeError, "Byte offset is not aligned.");
        }
        if ((view->byte_offset + view->byte_length) > view->byte_length)
            rb_raise(rb_eRangeError, "Byte offset / length is not aligned.");
        view->buf = obj;
    } else {
        rb_raise(rb_eTypeError, "XXX");
    }
    rb_obj_call_init(data_view, 0, NULL);
    return data_view;
}

static VALUE rb_data_view_byte_length(VALUE obj)
{
    GetDataView(obj);
    return ULONG2NUM(view->byte_length);
}

static VALUE rb_data_view_byte_offset(VALUE obj)
{
    GetDataView(obj);
    return ULONG2NUM(view->byte_offset);
}

static VALUE rb_data_view_buffer(VALUE obj)
{
    GetDataView(obj);
    return view->buf; 
}

static VALUE rb_data_view_set_int8(VALUE obj, VALUE offset_, VALUE item_)
{
    int argc;
    VALUE argv[3];
    argc = 3;
    argv[0] = offset_;
    argv[1] = item_;
    argv[2] = Qfalse;
    DataViewAset(obj, idx);
    rb_type_array_set_int8(buf->buf, offset, NUM2CHR(item), little_endian);
    return Qnil;
}

static VALUE rb_data_view_get_int8(VALUE obj, VALUE offset_)
{
    int argc;
    VALUE argv[2];
    argc = 1;
    argv[0] = offset_;
    argv[1] = Qfalse;
    DataViewAget(obj, idx);
    return CHR2FIX(rb_type_array_get_int8(buf->buf, offset, little_endian));
}

static VALUE rb_data_view_set_uint8(VALUE obj, VALUE offset_, VALUE item_)
{
    int argc;
    VALUE argv[3];
    argc = 3;
    argv[0] = offset_;
    argv[1] = item_;
    argv[2] = Qfalse;
    DataViewAset(obj, idx);
    rb_type_array_set_uint8(buf->buf, offset, (unsigned char)NUM2CHR(item), little_endian);
    return Qnil;
}

static VALUE rb_data_view_get_uint8(VALUE obj, VALUE offset_)
{
    int argc;
    VALUE argv[2];
    argc = 1;
    argv[0] = offset_;
    argv[1] = Qfalse;
    DataViewAget(obj, idx);
    return CHR2FIX(rb_type_array_get_uint8(buf->buf, offset, little_endian));
}

static VALUE rb_data_view_set_int16(int argc, VALUE *argv, VALUE obj)
{
    DataViewAset(obj, idx);
    rb_type_array_set_int16(buf->buf, offset, (short)NUM2INT(item), little_endian);
    return Qnil;
}

static VALUE rb_data_view_get_int16(int argc, VALUE *argv, VALUE obj)
{
    DataViewAget(obj, idx);
    return INT2FIX(rb_type_array_get_int16(buf->buf, offset, little_endian));
}

static VALUE rb_data_view_set_uint16(int argc, VALUE *argv, VALUE obj)
{
    DataViewAset(obj, idx);
    rb_type_array_set_uint16(buf->buf, offset, (unsigned short)NUM2INT(item), little_endian);
    return Qnil;
}

static VALUE rb_data_view_get_uint16(int argc, VALUE *argv, VALUE obj)
{
    DataViewAget(obj, idx);
    return INT2FIX(rb_type_array_get_uint16(buf->buf, offset, little_endian));
}

static VALUE rb_data_view_set_int32(int argc, VALUE *argv, VALUE obj)
{
    DataViewAset(obj, idx);
    rb_type_array_set_int32(buf->buf, offset, NUM2INT(item), little_endian);
    return Qnil;
}

static VALUE rb_data_view_get_int32(int argc, VALUE *argv, VALUE obj)
{
    DataViewAget(obj, idx);
    return INT2FIX(rb_type_array_get_int32(buf->buf, offset, little_endian));
}

static VALUE rb_data_view_set_uint32(int argc, VALUE *argv, VALUE obj)
{
    DataViewAset(obj, idx);
    rb_type_array_set_uint32(buf->buf, offset, (unsigned int)NUM2INT(item), little_endian);
    return Qnil;
}

static VALUE rb_data_view_get_uint32(int argc, VALUE *argv, VALUE obj)
{
    DataViewAget(obj, idx);
    return INT2FIX(rb_type_array_get_uint32(buf->buf, offset, little_endian));
}

static VALUE rb_data_view_set_float32(int argc, VALUE *argv, VALUE obj)
{
    float val;
    DataViewAset(obj, idx);
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
    rb_type_array_set_float32(buf->buf, offset, val, little_endian);
    return Qnil;
}

static VALUE rb_data_view_get_float32(int argc, VALUE *argv, VALUE obj)
{
    DataViewAget(obj, idx);
    return rb_float_new((double)rb_type_array_get_float32(buf->buf, offset, little_endian));
}

static VALUE rb_data_view_set_float64(int argc, VALUE *argv, VALUE obj)
{
    double val;
    DataViewAset(obj, idx);
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
    rb_type_array_set_float64(buf->buf, offset, val, little_endian);
    return Qnil;
}

static VALUE rb_data_view_get_float64(int argc, VALUE *argv, VALUE obj)
{
    DataViewAget(obj, idx);
    return rb_float_new(rb_type_array_get_float64(buf->buf, offset, little_endian));
}

void _init_data_view()
{
    rb_cDataView = rb_define_class("DataView", rb_cObject);

    rb_define_singleton_method(rb_cDataView, "new", rb_data_view_s_new, -1);
    rb_define_method(rb_cDataView, "byte_length", rb_data_view_byte_length, 0);
    rb_define_method(rb_cDataView, "byte_offset", rb_data_view_byte_offset, 0);
    rb_define_method(rb_cDataView, "buffer", rb_data_view_buffer, 0);

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