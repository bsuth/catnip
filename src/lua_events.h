#ifndef CATNIP_LUA_EVENTS_H
#define CATNIP_LUA_EVENTS_H

#include "extensions/lua.h"

extern lua_Ref catnip_lua_subscriptions;

void
catnip_lua_events_init(lua_State* L);

void
catnip_lua_events_subscribe(
  lua_State* L,
  lua_Ref subscriptions,
  const char* event
);

void
catnip_lua_events_unsubscribe(
  lua_State* L,
  lua_Ref subscriptions,
  const char* event
);

void
catnip_lua_events_publish(
  lua_State* L,
  lua_Ref subscriptions,
  const char* event,
  int nargs
);

#endif
