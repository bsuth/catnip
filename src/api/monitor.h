#ifndef CATNIP_API_MONITOR_H
#define CATNIP_API_MONITOR_H

#include "desktop/monitor.h"
#include <lua.h>

void
create_api_desktop_monitor(struct desktop_monitor* desktop_monitor);

void
init_api_monitors(lua_State* L);

#endif
