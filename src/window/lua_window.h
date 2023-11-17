#ifndef CATNIP_LUA_WINDOW_H
#define CATNIP_LUA_WINDOW_H

#include "utils/lua.h"
#include "window.h"

extern lua_Ref lua_catnip_windows;

void
lua_catnip_window_create(struct catnip_window* window);

void
lua_catnip_window_destroy(struct catnip_window* window);

void
lua_catnip_window_init(lua_State* L);

#endif
