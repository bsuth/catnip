#ifndef CATNIP_LUA_CURSOR_METHODS_H
#define CATNIP_LUA_CURSOR_METHODS_H

#include <lua.h>

int
lua_catnip_cursor_method_subscribe(lua_State* L);

int
lua_catnip_cursor_method_unsubscribe(lua_State* L);

int
lua_catnip_cursor_method_publish(lua_State* L);

#endif
