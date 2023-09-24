#ifndef CATNIP_LUA_API_UTILS_H
#define CATNIP_LUA_API_UTILS_H

#include <lua.h>

const char*
get_arg_error_msg(lua_State* L, const int arg_index, const char* details);

const char*
get_arg_type_error_msg(lua_State* L, int arg_index, const int expected_type);

#endif
