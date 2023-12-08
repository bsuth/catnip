#ifndef CATNIP_LUA_WINDOW_EVENTS_H
#define CATNIP_LUA_WINDOW_EVENTS_H

#include "window/window.h"
#include <lua.h>

int
lua_catnip_window_subscribe(lua_State* L);

int
lua_catnip_window_unsubscribe(lua_State* L);

int
lua_catnip_window_publish(lua_State* L);

void
lua_catnip_window_call_publish(
  lua_State* L,
  struct catnip_window* window,
  const char* event,
  int nargs
);

#endif
