#ifndef CATNIP_API_EVENTS_H
#define CATNIP_API_EVENTS_H

#include <lua.h>

int
api_publish(lua_State* L);

int
api_subscribe(lua_State* L);

int
api_unsubscribe(lua_State* L);

#endif
