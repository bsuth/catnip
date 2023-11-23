#include "log.h"
#include <glib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

void
log_log(const char* label, const char* format, ...)
{
  char timestamp[9];
  time_t timer = time(NULL);
  struct tm* tm = localtime(&timer);
  strftime(timestamp, sizeof(timestamp), "%H:%M:%S", tm);

  GString* message = g_string_new(NULL);
  va_list varargs;
  va_start(varargs, format);
  g_string_vprintf(message, format, varargs);
  va_end(varargs);

  printf("%s [%s] %s\n", label, timestamp, message->str);
  g_string_free(message, true);
}
