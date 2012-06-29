#ifndef TYPE_ARRAY_RUBY19_H
#define TYPE_ARRAY_RUBY19_H

#include <ruby/encoding.h>

extern rb_encoding *binary_encoding;
#define ArrayBufferEncode(str) rb_enc_associate(str, binary_encoding)

#define TRAP_BEG
#define TRAP_END

#endif