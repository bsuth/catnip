#ifndef CATNIP_API_CURSOR_H
#define CATNIP_API_CURSOR_H

#include "utils/lua.h"
#include <lua.h>

extern lua_Ref api_cursor_ref;

void
api_cursor_init(lua_State* L);

#endif
