#ifndef CATNIP_API_H
#define CATNIP_API_H

#include "utils/lua.h"
#include <lua.h>

extern lua_Ref api_catnip;

void
init_api(lua_State* L);

#endif
