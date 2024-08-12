#ifndef CATNIP_DESKTOP_LUA_KEYBOARD_H
#define CATNIP_DESKTOP_LUA_KEYBOARD_H

#include "desktop/keyboard.h"
#include "lua_resource.h"

struct catnip_lua_resource*
lua_catnip_keyboard_create(lua_State* L, struct catnip_keyboard* keyboard);

void
lua_catnip_keyboard_destroy(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
);

#endif
