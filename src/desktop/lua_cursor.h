#ifndef CATNIP_DESKTOP_LUA_CURSOR_H
#define CATNIP_DESKTOP_LUA_CURSOR_H

#include "lua_resource.h"
#include <lua.h>
#include <wlr/types/wlr_pointer.h>

extern struct catnip_lua_resource* catnip_lua_cursor;

void
catnip_lua_cursor_init(lua_State* L);

void
catnip_lua_cursor_publish_button_event(
  lua_State* L,
  struct wlr_pointer_button_event* event
);

#endif
