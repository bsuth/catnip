#ifndef CATNIP_DESKTOP_LUA_KEYBOARD_H
#define CATNIP_DESKTOP_LUA_KEYBOARD_H

#include "desktop/keyboard.h"
#include "lua_resource.h"

struct catnip_lua_resource*
catnip_lua_keyboard_create(lua_State* L, struct catnip_keyboard* keyboard);

void
catnip_lua_keyboard_destroy(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
);

void
catnip_lua_keyboard_publish(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* event,
  int nargs
);

#endif
