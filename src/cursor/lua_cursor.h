#ifndef CATNIP_LUA_CURSOR_H
#define CATNIP_LUA_CURSOR_H

#include "lua_resource.h"
#include <wlr/types/wlr_pointer.h>

extern struct catnip_lua_resource* lua_catnip_cursor;

void
lua_catnip_cursor_publish_button_event(
  lua_State* L,
  struct wlr_pointer_button_event* event
);

void
lua_catnip_cursor_init(lua_State* L);

#endif
