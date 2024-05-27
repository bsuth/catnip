#ifndef CATNIP_LUA_STATE_H
#define CATNIP_LUA_STATE_H

#include <lua.h>

void
lua_catnip_state_init(lua_State* L);

void
lua_catnip_state_populate(lua_State* L);

#endif
