#include "string.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool
streq(const char* a, const char* b)
{
  return strcmp(a, b) == 0;
}

char*
strvfmt(const char* format, va_list varargs)
{
  va_list varlen;
  va_copy(varlen, varargs);
  int len = 1 + vsnprintf(NULL, 0, format, varlen);

  char* str = malloc(len * sizeof(char));
  vsprintf(str, format, varargs);

  return str;
}

char*
strfmt(const char* format, ...)
{
  va_list varargs;
  va_start(varargs, format);
  char* str = strvfmt(format, varargs);
  va_end(varargs);
  return str;
}
