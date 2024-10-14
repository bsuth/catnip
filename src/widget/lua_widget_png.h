#ifndef CATNIP_LUA_WIDGET_PNG_H
#define CATNIP_LUA_WIDGET_PNG_H

#include "extensions/lua.h"
#include "widget/lua_widget_base.h"
#include <cairo.h>

struct catnip_lua_widget_png {
  // IMPORTANT: Keep `base` at the start to allow pointer casting.
  struct catnip_lua_widget_base base;

  lua_Ref path;
  cairo_surface_t* surface;
  double aspect_ratio;
};

void
catnip_lua_widget_png_init(lua_State* L);

int
catnip_lua_widget_lua_png(lua_State* L);

void
catnip_lua_widget_png_draw(
  lua_State* L,
  struct catnip_lua_widget_png* png,
  cairo_t* cr
);

#endif
