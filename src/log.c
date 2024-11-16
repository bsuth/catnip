#include "log.h"
#include "config.h"
#include "extensions/string.h"
#include <lauxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum CATNIP_LOG_LEVEL catnip_log_level = CATNIP_LOG_LEVEL_ERROR;

static const char*
catnip_log_level_label(enum CATNIP_LOG_LEVEL log_level)
{
  switch (log_level) {
    case CATNIP_LOG_LEVEL_ERROR:
      return "ERROR";
    case CATNIP_LOG_LEVEL_WARNING:
      return "WARNING";
    case CATNIP_LOG_LEVEL_INFO:
      return "INFO";
    case CATNIP_LOG_LEVEL_DEBUG:
      return "DEBUG";
    default:
      return "UNKNOWN";
  }
}

void
catnip_log(enum CATNIP_LOG_LEVEL log_level, const char* format, ...)
{
  if (log_level > catnip_log_level) {
    return; // nothing to do
  }

  char timestamp[9];
  time_t timer = time(NULL);
  struct tm* tm = localtime(&timer);
  strftime(timestamp, sizeof(timestamp), "%H:%M:%S", tm);

  va_list varargs;
  va_start(varargs, format);
  char* message = strvfmt(format, varargs);
  va_end(varargs);

  lua_Debug caller_info;

  if (catnip_L == NULL || !lua_getstack(catnip_L, 1, &caller_info)) {
    printf("[%s] %s\n", catnip_log_level_label(log_level), message);
  } else {
    lua_getinfo(catnip_L, "Sl", &caller_info);

    printf(
      "[%s] %s:%d: %s\n",
      catnip_log_level_label(log_level),
      caller_info.short_src,
      caller_info.currentline,
      message
    );
  }

  free(message);
}
