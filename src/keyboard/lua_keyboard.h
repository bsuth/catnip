#ifndef CATNIP_LUA_KEYBOARD_H
#define CATNIP_LUA_KEYBOARD_H

#include "keyboard/keyboard.h"
#include "lua_resource.h"

struct catnip_lua_resource*
lua_catnip_keyboard_create(lua_State* L, struct catnip_keyboard* keyboard);

void
lua_catnip_keyboard_destroy(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
);

#endif
