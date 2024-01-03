#ifndef CATNIP_LUA_CURSOR_EVENTS_H
#define CATNIP_LUA_CURSOR_EVENTS_H

#include <lua.h>

int
lua_catnip_cursor_subscribe(lua_State* L);

int
lua_catnip_cursor_unsubscribe(lua_State* L);

int
lua_catnip_cursor_publish(lua_State* L);

void
lua_catnip_cursor_call_publish(lua_State* L, const char* event, int nargs);

#endif
