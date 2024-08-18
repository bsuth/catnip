#ifndef CATNIP_DESKTOP_LUA_KEYBOARD_H
#define CATNIP_DESKTOP_LUA_KEYBOARD_H

#include "desktop/keyboard.h"
#include "extensions/lua.h"

struct catnip_lua_keyboard {
  lua_Ref ref;
  lua_Ref subscriptions;
  struct wl_list link;
  struct catnip_keyboard* keyboard;
};

void
catnip_lua_keyboard_init(lua_State* L);

void
catnip_lua_keyboard_create(lua_State* L, struct catnip_keyboard* keyboard);

void
catnip_lua_keyboard_destroy(
  lua_State* L,
  struct catnip_lua_keyboard* lua_keyboard
);

void
catnip_lua_keyboard_publish(
  lua_State* L,
  struct catnip_lua_keyboard* lua_keyboard,
  const char* event,
  int nargs
);

#endif
