#ifndef CATNIP_LUA_KEY_EVENT_H
#define CATNIP_LUA_KEY_EVENT_H

#include "input/keyboard.h"
#include <lua.h>
#include <stdbool.h>

void
lua_catnip_key_event_init(lua_State* L);

void
lua_catnip_publish_key_event(struct catnip_key_event* event);

#endif
