#ifndef CATNIP_UTILS_LOG_H
#define CATNIP_UTILS_LOG_H

enum LOG_LEVEL {
  LOG_LEVEL_SILENT,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_INFO,
  LOG_LEVEL_DEBUG,
};

extern enum LOG_LEVEL global_log_level;

void
log_log(enum LOG_LEVEL log_level, const char* format, ...);

#define log_error(...) log_log(LOG_LEVEL_ERROR, __VA_ARGS__)
#define log_warning(...) log_log(LOG_LEVEL_WARNING, __VA_ARGS__)
#define log_info(...) log_log(LOG_LEVEL_INFO, __VA_ARGS__)
#define log_debug(...) log_log(LOG_LEVEL_DEBUG, __VA_ARGS__)

#endif
