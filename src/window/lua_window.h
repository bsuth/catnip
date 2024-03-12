#ifndef CATNIP_LUA_WINDOW_H
#define CATNIP_LUA_WINDOW_H

#include "lua_resource.h"
#include "window/window.h"
#include <lua.h>

struct catnip_lua_resource*
lua_catnip_window_create(lua_State* L, struct catnip_window* window);

void
lua_catnip_window_destroy(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
);

#endif
