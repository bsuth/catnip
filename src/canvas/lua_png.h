#ifndef CATNIP_LUA_PNG_H
#define CATNIP_LUA_PNG_H

#include <cairo.h>
#include <lua.h>

struct lua_catnip_png {
  char* path;
  double width;
  double height;
  cairo_surface_t* surface;
};

void
lua_catnip_png_init(lua_State* L);

int
lua_catnip_png(lua_State* L);

int
lua_catnip_canvas_png(lua_State* L);

#endif
