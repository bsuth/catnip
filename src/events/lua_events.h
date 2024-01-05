#ifndef CATNIP_LUA_EVENTS_H
#define CATNIP_LUA_EVENTS_H

#include "utils/lua.h"

extern lua_Ref lua_catnip_subscriptions;

void
lua_catnip_events_subscribe(
  lua_State* L,
  lua_Ref subscriptions,
  const char* event
);

void
lua_catnip_events_unsubscribe(
  lua_State* L,
  lua_Ref subscriptions,
  const char* event
);

void
lua_catnip_events_publish(
  lua_State* L,
  lua_Ref subscriptions,
  const char* event,
  int nargs
);

void
lua_catnip_events_init(lua_State* L);

#endif
