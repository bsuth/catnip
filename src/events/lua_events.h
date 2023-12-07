#ifndef CATNIP_LUA_EVENTS_H
#define CATNIP_LUA_EVENTS_H

#include <lua.h>

int
lua_catnip_events_subscribe(lua_State* L);

int
lua_catnip_events_global_subscribe(lua_State* L);

int
lua_catnip_events_unsubscribe(lua_State* L);

int
lua_catnip_events_global_unsubscribe(lua_State* L);

int
lua_catnip_events_publish(lua_State* L);

int
lua_catnip_events_global_publish(lua_State* L);

void
lua_catnip_events_call_publish(lua_State* L, const char* event, int nargs);

void
lua_catnip_events_init(lua_State* L);

#endif
