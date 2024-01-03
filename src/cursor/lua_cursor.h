#ifndef CATNIP_LUA_CURSOR_H
#define CATNIP_LUA_CURSOR_H

#include "utils/lua.h"
#include <wlr/types/wlr_pointer.h>

extern lua_Ref lua_catnip_cursor;
extern lua_Ref lua_catnip_cursor_subscriptions;

void
lua_catnip_cursor_publish_button_event(
  lua_State* L,
  struct wlr_pointer_button_event* event
);

void
lua_catnip_cursor_init(lua_State* L);

#endif
