#ifndef CATNIP_LUA_WIDGET_SVG_H
#define CATNIP_LUA_WIDGET_SVG_H

#include "extensions/lua.h"
#include "widget/lua_widget_base.h"
#include <cairo.h>
#include <librsvg/rsvg.h>

struct catnip_lua_widget_svg {
  // IMPORTANT: Keep `base` at the start to allow pointer casting.
  struct catnip_lua_widget_base base;

  RsvgHandle* rsvg;
  double aspect_ratio;

  struct {
    const char* document;
    lua_Ref document_ref;
    const char* stylesheet;
    lua_Ref stylesheet_ref;
  } styles;
};

void
catnip_lua_widget_svg_init(lua_State* L);

int
catnip_lua_widget_lua_svg(lua_State* L);

void
catnip_lua_widget_svg_draw(
  lua_State* L,
  struct catnip_lua_widget_svg* svg,
  cairo_t* cr
);

#endif
