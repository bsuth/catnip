#ifndef CATNIP_LUA_WINDOW_METHODS_H
#define CATNIP_LUA_WINDOW_METHODS_H

#include <lua.h>

int
lua_catnip_window_method_move(lua_State* L);

int
lua_catnip_window_method_resize(lua_State* L);

int
lua_catnip_window_method_destroy(lua_State* L);

#endif