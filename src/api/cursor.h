#ifndef CATNIP_API_CURSOR_H
#define CATNIP_API_CURSOR_H

#include "utils/lua.h"
#include <lua.h>

extern lua_Ref api_catnip_cursor;

void
init_api_cursor(lua_State* L);

#endif
