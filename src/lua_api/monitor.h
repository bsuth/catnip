#ifndef CATNIP_LUA_API_MONITOR_H
#define CATNIP_LUA_API_MONITOR_H

#include "desktop/monitor.h"
#include <lua.h>

void
create_lua_desktop_monitor(struct desktop_monitor* desktop_monitor);

void
init_lua_monitors(lua_State* L);

#endif
