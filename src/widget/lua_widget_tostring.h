#ifndef CATNIP_LUA_WIDGET_TOSTRING_H
#define CATNIP_LUA_WIDGET_TOSTRING_H

#include "extensions/lua.h"
#include "widget/lua_widget_base.h"
#include <cairo.h>
#include <pango/pango-layout.h>

struct catnip_lua_widget_tostring {
  // IMPORTANT: Keep `base` at the start to allow pointer casting.
  struct catnip_lua_widget_base base;

  PangoContext* context;
  PangoLayout* layout;

  lua_Ref ref;
  const char* text;
};

void
catnip_lua_widget_tostring_init(lua_State* L);

struct catnip_lua_widget_tostring*
catnip_lua_widget_lua_tostring(lua_State* L, int idx);

void
catnip_lua_widget_tostring_draw(
  lua_State* L,
  struct catnip_lua_widget_tostring* tostring,
  cairo_t* cr
);

#endif
