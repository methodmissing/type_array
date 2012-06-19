#include <type_array_ext.h>

DefineTypeAccessor(int8, char);
DefineTypeAccessor(uint8, unsigned char);
DefineTypeAccessor(int16, short);
DefineTypeAccessor(uint16, unsigned short);
DefineTypeAccessor(int32, int);
DefineTypeAccessor(uint32, unsigned int);
DefineTypeAccessor(float32, float);
DefineTypeAccessor(float64, double);

void Init_type_array_ext()
{
    _init_array_buffer();
    _init_type_array();
    _init_data_view();
}