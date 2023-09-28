#ifndef CATNIP_API_WINDOW_H
#define CATNIP_API_WINDOW_H

#include "desktop/window.h"
#include <lua.h>

void
create_api_desktop_window(struct desktop_window* desktop_window);

void
init_api_windows(lua_State* L);

#endif
