#include "log.h"
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

enum LOG_LEVEL global_log_level = LOG_LEVEL_ERROR;

static const char*
get_log_level_label(enum LOG_LEVEL log_level)
{
  switch (log_level) {
    case LOG_LEVEL_ERROR:
      return "ERROR";
    case LOG_LEVEL_WARNING:
      return "WARNING";
    case LOG_LEVEL_INFO:
      return "INFO";
    case LOG_LEVEL_DEBUG:
      return "DEBUG";
    default:
      return "UNKNOWN";
  }
}

void
log_log(enum LOG_LEVEL log_level, const char* format, ...)
{
  if (log_level > global_log_level) {
    return; // nothing to do
  }

  char timestamp[9];
  time_t timer = time(NULL);
  struct tm* tm = localtime(&timer);
  strftime(timestamp, sizeof(timestamp), "%H:%M:%S", tm);

  GString* message = g_string_new(NULL);
  va_list varargs;
  va_start(varargs, format);
  g_string_vprintf(message, format, varargs);
  va_end(varargs);

  printf(
    "[%s] %s: %s\n",
    get_log_level_label(log_level),
    timestamp,
    message->str
  );

  g_string_free(message, true);
}
