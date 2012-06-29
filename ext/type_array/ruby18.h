#ifndef TYPE_ARRAY_RUBY18_H
#define TYPE_ARRAY_RUBY18_H

#define ArrayBufferEncode(str) str

#ifndef RSTRING_PTR
#define RSTRING_PTR(str) RSTRING(str)->ptr
#endif
#ifndef RSTRING_LEN
#define RSTRING_LEN(s) (RSTRING(s)->len)
#endif

#include "rubysig.h"

#endif