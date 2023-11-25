#ifndef CATNIP_LUA_EVENTS_H
#define CATNIP_LUA_EVENTS_H

#include <lua.h>

int
catnip_lua_events_subscribe(lua_State* L);

int
catnip_lua_events_unsubscribe(lua_State* L);

int
catnip_lua_events_publish(lua_State* L);

void
catnip_lua_events_init(lua_State* L);

#endif
