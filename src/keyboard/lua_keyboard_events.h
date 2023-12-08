#ifndef CATNIP_LUA_KEYBOARD_EVENTS_H
#define CATNIP_LUA_KEYBOARD_EVENTS_H

#include "keyboard/keyboard.h"
#include <lua.h>

int
lua_catnip_keyboard_subscribe(lua_State* L);

int
lua_catnip_keyboard_unsubscribe(lua_State* L);

int
lua_catnip_keyboard_publish(lua_State* L);

void
lua_catnip_keyboard_call_publish(
  lua_State* L,
  struct catnip_keyboard* keyboard,
  const char* event,
  int nargs
);

#endif
