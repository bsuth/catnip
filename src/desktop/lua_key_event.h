#ifndef CATNIP_DESKTOP_LUA_KEY_EVENT_H
#define CATNIP_DESKTOP_LUA_KEY_EVENT_H

#include "desktop/key_event.h"
#include "desktop/keyboard.h"
#include <lua.h>

void
lua_catnip_publish_key_event(
  lua_State* L,
  struct catnip_keyboard* keyboard,
  struct catnip_key_event* event
);

#endif
