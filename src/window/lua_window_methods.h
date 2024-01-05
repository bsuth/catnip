#ifndef CATNIP_LUA_WINDOW_METHODS_H
#define CATNIP_LUA_WINDOW_METHODS_H

#include <lua.h>

int
lua_catnip_window_method_subscribe(lua_State* L);

int
lua_catnip_window_method_unsubscribe(lua_State* L);

int
lua_catnip_window_method_publish(lua_State* L);

int
lua_catnip_window_method_destroy(lua_State* L);

#endif
