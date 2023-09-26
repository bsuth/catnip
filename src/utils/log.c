#include "log.h"
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

void
log_log(const char* label, const char* format, ...)
{
  char timestamp[9];
  time_t timer = time(NULL);
  struct tm* tm = localtime(&timer);
  strftime(timestamp, sizeof(timestamp), "%H:%M:%S", tm);

  char message[256];
  va_list argp;
  va_start(argp, format);
  vsnprintf(message, sizeof(message), format, argp);
  va_end(argp);

  printf("%s %s: %s\n", timestamp, label, message);
}
