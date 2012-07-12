#include <type_array_ext.h>

#ifdef HAVE_RUBY_ENCODING_H
rb_encoding *binary_encoding;
#endif

DefineTypeAccessor(int8, signed char);
DefineTypeAccessor(uint8, unsigned char);
DefineTypeAccessor(int16, short);
DefineTypeAccessor(uint16, unsigned short);
DefineTypeAccessor(int32, int);
DefineTypeAccessor(uint32, unsigned int);
DefineTypeAccessor(float32, float);
DefineTypeAccessor(float64, double);

void Init_type_array_ext()
{
#ifdef HAVE_RUBY_ENCODING_H
    binary_encoding = rb_enc_find("binary");
#endif

    _init_array_buffer();
    _init_type_array();
    _init_data_view();
}