#ifndef CATNIP_API_KEYBINDINGS_H
#define CATNIP_API_KEYBINDINGS_H

#include <lua.h>

int
api_bind(lua_State* L);

int
api_unbind(lua_State* L);

#endif
