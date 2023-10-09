#ifndef CATNIP_API_OUTPUT_H
#define CATNIP_API_OUTPUT_H

#include "utils/lua.h"
#include <lua.h>

extern lua_Ref api_catnip_outputs;

void
init_api_outputs(lua_State* L);

#endif
