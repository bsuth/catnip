#ifndef CATNIP_LUA_API_WINDOWS_H
#define CATNIP_LUA_API_WINDOWS_H

#include "server/desktop/windows.h"
#include <lua.h>

void
create_lua_desktop_window(struct desktop_window* desktop_window);

void
init_lua_windows(lua_State* L);

#endif
