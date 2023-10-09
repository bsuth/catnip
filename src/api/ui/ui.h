#ifndef CATNIP_API_UI_H
#define CATNIP_API_UI_H

#include "utils/lua.h"
#include <lua.h>

extern lua_Ref api_catnip_ui;

void
init_api_ui(lua_State* L);

#endif
