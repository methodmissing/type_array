#ifndef TYPE_ARRAY_RUBY19_H
#define TYPE_ARRAY_RUBY19_H

#include <ruby/io.h>

#define GetWriteFile(fptr) rb_io_stdio_file(fptr)
#define GetReadFile(fptr) rb_io_stdio_file(fptr)

#define TRAP_BEG
#define TRAP_END

#endif