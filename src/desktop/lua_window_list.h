#ifndef CATNIP_DESKTOP_LUA_WINDOW_LIST_H
#define CATNIP_DESKTOP_LUA_WINDOW_LIST_H

#include "lua_resource_list.h"
#include <lua.h>

extern struct catnip_lua_resource_list* catnip_lua_window_list;

void
catnip_lua_window_list_init(lua_State* L);

void
catnip_lua_window_list_populate(lua_State* L);

#endif
