#ifndef CATNIP_LUA_PNG_H
#define CATNIP_LUA_PNG_H

#include "canvas/canvas.h"
#include <cairo.h>
#include <lua.h>

struct lua_catnip_png {
  char* path;
  double width;
  double height;
  cairo_surface_t* surface;
};

int
lua_catnip_png(lua_State* L);

void
lua_catnip_png_render(
  struct lua_catnip_png* lua_png,
  struct catnip_canvas* canvas,
  double x,
  double y,
  double width,
  double height
);

void
lua_catnip_png_init(lua_State* L);

#endif
