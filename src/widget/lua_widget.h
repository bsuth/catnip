#ifndef CATNIP_LUA_WIDGET_H
#define CATNIP_LUA_WIDGET_H

#include "extensions/lua.h"
#include <wayland-util.h>

extern lua_Ref catnip_lua_widget;

void
catnip_lua_widget_init(lua_State* L);

#endif
