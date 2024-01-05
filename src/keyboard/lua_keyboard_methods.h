#ifndef CATNIP_LUA_KEYBOARD_METHODS_H
#define CATNIP_LUA_KEYBOARD_METHODS_H

#include <lua.h>

int
lua_catnip_keyboard_method_subscribe(lua_State* L);

int
lua_catnip_keyboard_method_unsubscribe(lua_State* L);

int
lua_catnip_keyboard_method_publish(lua_State* L);

#endif
