#ifndef CATNIP_LUA_OUTPUT_MODE_LIST_H
#define CATNIP_LUA_OUTPUT_MODE_LIST_H

#include "lua_resource_list.h"
#include "output/output.h"
#include <lua.h>

struct catnip_lua_resource_list*
lua_catnip_output_mode_list_create(lua_State* L, struct catnip_output* output);

#endif
