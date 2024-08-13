#ifndef CATNIP_LUA_STATE_H
#define CATNIP_LUA_STATE_H

#include <lua.h>

void
catnip_lua_state_init(lua_State* L);

void
catnip_lua_state_populate(lua_State* L);

#endif
