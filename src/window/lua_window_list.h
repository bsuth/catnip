#ifndef CATNIP_LUA_WINDOW_LIST_H
#define CATNIP_LUA_WINDOW_LIST_H

#include "lua_resource_list.h"
#include <lua.h>

extern struct catnip_lua_resource_list* lua_catnip_window_list;

void
lua_catnip_window_list_init(lua_State* L);

#endif
