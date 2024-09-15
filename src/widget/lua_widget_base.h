#ifndef CATNIP_LUA_WIDGET_BASE_H
#define CATNIP_LUA_WIDGET_BASE_H

#include "widget/widget_base.h"
#include <lua.h>

struct catnip_lua_widget_base_mt {
  int (*__index)(lua_State* L, struct catnip_widget_base* base);
  void (*__newindex)(lua_State* L, struct catnip_widget_base* base);
  void (*__gc)(lua_State* L, struct catnip_widget_base* base);
};

void
catnip_lua_widget_base_init(lua_State* L);

struct catnip_widget_base*
catnip_lua_widget_base(lua_State* L);

#endif
