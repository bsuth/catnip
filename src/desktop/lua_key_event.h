#ifndef CATNIP_DESKTOP_LUA_KEY_EVENT_H
#define CATNIP_DESKTOP_LUA_KEY_EVENT_H

#include "desktop/key_event.h"
#include "desktop/keyboard.h"
#include <lua.h>

struct catnip_lua_key_event {
  struct catnip_key_event* key_event;
};

void
catnip_lua_key_event_init(lua_State* L);

void
catnip_lua_key_event_publish(
  lua_State* L,
  struct catnip_keyboard* keyboard,
  struct catnip_key_event* key_event
);

#endif
