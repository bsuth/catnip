#ifndef CATNIP_LUA_WINDOW_H
#define CATNIP_LUA_WINDOW_H

#include "window.h"

extern lua_Ref lua_catnip_windows;

void
lua_catnip_window_destroy(lua_State* L, struct catnip_window* window);

void
lua_catnip_window_create(lua_State* L, struct catnip_window* window);

void
lua_catnip_window_init(lua_State* L);

#endif
