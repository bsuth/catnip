#ifndef CATNIP_DESKTOP_LUA_WINDOW_H
#define CATNIP_DESKTOP_LUA_WINDOW_H

#include "desktop/window.h"
#include "lua_resource.h"
#include <lua.h>

struct catnip_lua_resource*
catnip_lua_window_create(lua_State* L, struct catnip_window* window);

void
catnip_lua_window_destroy(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
);

void
catnip_lua_window_publish(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* event,
  int nargs
);

#endif
