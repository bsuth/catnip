#ifndef CATNIP_UTILS_STRING_H
#define CATNIP_UTILS_STRING_H

#include <stdarg.h>
#include <stdbool.h>

bool
streq(const char* a, const char* b);

char*
strvfmt(const char* format, va_list varargs);

char*
strfmt(const char* format, ...);

#endif
