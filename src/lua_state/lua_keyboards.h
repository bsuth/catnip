#ifndef CATNIP_LUA_STATE_LUA_KEYBOARDS_H
#define CATNIP_LUA_STATE_LUA_KEYBOARDS_H

#include "extensions/lua.h"

struct catnip_lua_keyboards_state {
  lua_Ref ref;
};

struct catnip_lua_keyboards_state*
catnip_lua_keyboards_state_create(lua_State* L);

#endif
