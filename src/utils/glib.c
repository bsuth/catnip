#include "glib.h"

GString*
g_string_new_printf(const char* format, ...)
{
  GString* s = g_string_new(NULL);
  va_list argp;
  va_start(argp, format);
  g_string_append_vprintf(s, format, argp);
  va_end(argp);
  return s;
}
