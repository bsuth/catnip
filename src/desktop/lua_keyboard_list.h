#ifndef CATNIP_DESKTOP_LUA_KEYBOARD_LIST_H
#define CATNIP_DESKTOP_LUA_KEYBOARD_LIST_H

#include "lua_resource_list.h"
#include <lua.h>

extern struct catnip_lua_resource_list* catnip_lua_keyboard_list;

void
catnip_lua_keyboard_list_init(lua_State* L);

void
catnip_lua_keyboard_list_populate(lua_State* L);

#endif
