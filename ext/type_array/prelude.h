#include <stdint.h>

#define TYPE_ARRAY_IS_LITTLE_ENDIAN (*(uint16_t*)"\0\1">>8) ? Qtrue : Qfalse

#ifndef RFLOAT_VALUE
#define RFLOAT_VALUE(v) (RFLOAT(v)->value)
#endif