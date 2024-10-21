#ifndef CATNIP_LUA_WIDGET_PNG_H
#define CATNIP_LUA_WIDGET_PNG_H

#include "extensions/lua.h"
#include "widget/lua_widget_base.h"
#include <cairo.h>

struct catnip_lua_widget_png {
  // IMPORTANT: Keep `base` at the start to allow pointer casting.
  struct catnip_lua_widget_base base;

  cairo_surface_t* surface;
  int intrinsic_width;
  int intrinsic_height;

  struct {
    const char* path;
    lua_Ref path_ref;
  } styles;
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
