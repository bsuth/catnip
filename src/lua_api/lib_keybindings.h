#ifndef CATNIP_LUA_API_LIB_KEYBINDINGS_H
#define CATNIP_LUA_API_LIB_KEYBINDINGS_H

#include <lua.h>

int
lib_keybindings_add(lua_State* L);

int
lib_keybindings_remove(lua_State* L);

int
lib_keybindings_clear(lua_State* L);

#endif
