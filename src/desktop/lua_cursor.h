#ifndef CATNIP_DESKTOP_LUA_CURSOR_H
#define CATNIP_DESKTOP_LUA_CURSOR_H

#include "extensions/lua.h"
#include <wlr/types/wlr_pointer.h>

struct catnip_lua_cursor {
  lua_Ref ref;
  lua_Ref subscriptions;
};

extern struct catnip_lua_cursor* catnip_lua_cursor;

void
catnip_lua_cursor_init(lua_State* L);

void
catnip_lua_cursor_publish_button_event(
  lua_State* L,
  struct wlr_pointer_button_event* event
);

#endif
