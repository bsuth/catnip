#ifndef CATNIP_API_WINDOW_H
#define CATNIP_API_WINDOW_H

#include "utils/lua.h"
#include <lua.h>

extern lua_Ref api_catnip_windows;

void
init_api_windows(lua_State* L);

#endif
