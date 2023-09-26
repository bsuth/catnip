#ifndef CATNIP_UTILS_LOG_H
#define CATNIP_UTILS_LOG_H

void
log_log(const char* label, const char* format, ...);

#define log_debug(...) log_log("DEBUG", __VA_ARGS__)
#define log_info(...) log_log("INFO", __VA_ARGS__)
#define log_warning(...) log_log("WARNING", __VA_ARGS__)
#define log_error(...) log_log("ERROR", __VA_ARGS__)

#endif
