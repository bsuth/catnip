#ifndef CATNIP_API_H
#define CATNIP_API_H

#include "utils/lua.h"
#include <lua.h>

extern lua_Ref api_ref;

void
api_init(lua_State* L);

#endif
