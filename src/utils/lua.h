#ifndef CATNIP_UTILS_LUA_H
#define CATNIP_UTILS_LUA_H

#include <lua.h>

const char*
lua_get_arg_error_msg(lua_State* L, const int arg_index, const char* details);

const char*
lua_get_arg_type_error_msg(
  lua_State* L,
  int arg_index,
  const int expected_type
);

#endif
