#ifndef CATNIP_DESKTOP_LUA_WINDOW_H
#define CATNIP_DESKTOP_LUA_WINDOW_H

#include "desktop/window.h"
#include "extensions/lua.h"

struct catnip_lua_window {
  lua_Ref ref;
  lua_Ref subscriptions;
  struct wl_list link;
  struct catnip_window* window;
};

void
catnip_lua_window_init(lua_State* L);

void
catnip_lua_window_create(lua_State* L, struct catnip_window* window);

void
catnip_lua_window_destroy(lua_State* L, struct catnip_lua_window* lua_window);

void
catnip_lua_window_publish(
  lua_State* L,
  struct catnip_lua_window* lua_window,
  const char* event,
  int nargs
);

#endif
