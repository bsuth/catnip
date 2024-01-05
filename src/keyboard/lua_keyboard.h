#ifndef CATNIP_LUA_KEYBOARD_H
#define CATNIP_LUA_KEYBOARD_H

#include "keyboard/keyboard.h"

extern lua_Ref lua_catnip_keyboards;

void
lua_catnip_keyboard_destroy(lua_State* L, struct catnip_keyboard* keyboard);

void
lua_catnip_keyboard_create(lua_State* L, struct catnip_keyboard* keyboard);

void
lua_catnip_keyboard_publish(
  lua_State* L,
  struct catnip_keyboard* keyboard,
  const char* event,
  int nargs
);

void
lua_catnip_keyboard_publish_key_event(
  lua_State* L,
  struct catnip_keyboard* keyboard,
  struct catnip_keyboard_key_event* event
);

void
lua_catnip_keyboard_init(lua_State* L);

#endif
