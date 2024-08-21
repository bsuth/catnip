#ifndef CATNIP_LUA_KEYBINDINGS_H
#define CATNIP_LUA_KEYBINDINGS_H

#include "desktop/key_event.h"
#include "extensions/lua.h"

extern lua_Ref catnip_lua_keybindings;

void
catnip_lua_keybindings_init(lua_State* L);

bool
catnip_lua_keybindings_check(lua_State* L, struct catnip_key_event* key_event);

int
catnip_lua_keybindings_bind(lua_State* L);

int
catnip_lua_keybindings_unbind(lua_State* L);

#endif
