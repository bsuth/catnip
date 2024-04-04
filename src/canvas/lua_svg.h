#ifndef CATNIP_LUA_SVG_H
#define CATNIP_LUA_SVG_H

#include "canvas/canvas.h"
#include <librsvg/rsvg.h>
#include <lua.h>

struct lua_catnip_svg {
  char* data;
  double width;
  double height;
  RsvgHandle* rsvg;
};

int
lua_catnip_svg(lua_State* L);

void
lua_catnip_svg_render(
  struct lua_catnip_svg* lua_svg,
  struct catnip_canvas* canvas,
  double x,
  double y,
  double width,
  double height
);

void
lua_catnip_svg_init(lua_State* L);

#endif
