#ifndef CATNIP_API_EVENTS_H
#define CATNIP_API_EVENTS_H

#include <lua.h>

int
api_listen(lua_State* L);

int
api_unlisten(lua_State* L);

#endif
