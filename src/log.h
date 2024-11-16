#ifndef CATNIP_LOG_H
#define CATNIP_LOG_H

#include <lua.h>

enum CATNIP_LOG_LEVEL {
  CATNIP_LOG_LEVEL_SILENT,
  CATNIP_LOG_LEVEL_ERROR,
  CATNIP_LOG_LEVEL_WARNING,
  CATNIP_LOG_LEVEL_INFO,
  CATNIP_LOG_LEVEL_DEBUG,
};

extern enum CATNIP_LOG_LEVEL catnip_log_level;

void
catnip_log(enum CATNIP_LOG_LEVEL log_level, const char* format, ...);

#define catnip_log_error(...) catnip_log(CATNIP_LOG_LEVEL_ERROR, __VA_ARGS__)
#define catnip_log_warning(...) \
  catnip_log(CATNIP_LOG_LEVEL_WARNING, __VA_ARGS__)
#define catnip_log_info(...) catnip_log(CATNIP_LOG_LEVEL_INFO, __VA_ARGS__)
#define catnip_log_debug(...) catnip_log(CATNIP_LOG_LEVEL_DEBUG, __VA_ARGS__)

// void
// catnip_lua_log(
//   lua_State* L,
//   enum CATNIP_LOG_LEVEL log_level,
//   const char* format,
//   ...
// );

// #define catnip_lua_log_error(L, ...) \
//   catnip_lua_log(L, CATNIP_LOG_LEVEL_ERROR, __VA_ARGS__)
// #define catnip_lua_log_warning(L, ...) \
//   catnip_lua_log(L, CATNIP_LOG_LEVEL_WARNING, __VA_ARGS__)
// #define catnip_lua_log_info(L, ...) \
//   catnip_lua_log(L, CATNIP_LOG_LEVEL_INFO, __VA_ARGS__)
// #define catnip_lua_log_debug(L, ...) \
//   catnip_lua_log(L, CATNIP_LOG_LEVEL_DEBUG, __VA_ARGS__)

// void
// catnip_lua_log_bad_arg_type(lua_State* L, int index);

// void
// catnip_lua_log_expected_arg_type(lua_State* L, int index, int type);

// void
// catnip_lua_log_bad_field_type(lua_State* L, int index, const char* field);

// void
// catnip_lua_log_expected_field_type(
//   lua_State* L,
//   int index,
//   const char* field,
//   int type
// );

#endif
