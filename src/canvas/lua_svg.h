#ifndef CATNIP_LUA_SVG_H
#define CATNIP_LUA_SVG_H

#include <librsvg/rsvg.h>
#include <lua.h>

struct lua_catnip_svg {
  char* document;
  double width;
  double height;
  RsvgHandle* rsvg;
};

void
lua_catnip_svg_init(lua_State* L);

int
lua_catnip_svg(lua_State* L);

int
lua_catnip_canvas_svg(lua_State* L);

#endif
