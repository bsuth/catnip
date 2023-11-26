#ifndef CATNIP_LUA_EVENTS_H
#define CATNIP_LUA_EVENTS_H

#include "utils/lua.h"

int
catnip_lua_events_local_subscribe(lua_State* L, lua_Ref subscriptions);

int
catnip_lua_events_global_subscribe(lua_State* L);

int
catnip_lua_events_local_unsubscribe(lua_State* L, lua_Ref subscriptions);

int
catnip_lua_events_global_unsubscribe(lua_State* L);

int
catnip_lua_events_local_publish(lua_State* L, lua_Ref subscriptions);

int
catnip_lua_events_global_publish(lua_State* L);

void
lua_catnip_events_call_publish(lua_State* L, const char* event, int nargs);

void
catnip_lua_events_init(lua_State* L);

#endif
