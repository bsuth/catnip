#ifndef CATNIP_LUA_WIDGET_BLOCK_H
#define CATNIP_LUA_WIDGET_BLOCK_H

#include "extensions/lua.h"
#include "widget/lua_widget_base.h"
#include <cairo.h>

struct catnip_lua_widget_block {
  // IMPORTANT: Keep `base` at the start to allow pointer casting.
  struct catnip_lua_widget_base base;

  lua_Ref children;

  struct {
    lua_Ref layout;

    int padding;
    int padding_top;
    int padding_left;
    int padding_bottom;
    int padding_right;

    int radius;
    int radius_top_left;
    int radius_top_right;
    int radius_bottom_right;
    int radius_bottom_left;

    int bg_color;
    double bg_opacity;

    int border_color;
    double border_opacity;
    int border_width;
  } styles;
};

void
catnip_lua_widget_block_init(lua_State* L);

int
catnip_lua_widget_lua_block(lua_State* L);

void
catnip_lua_widget_block_layout(
  lua_State* L,
  struct catnip_lua_widget_block* block
);

void
catnip_lua_widget_block_draw(
  lua_State* L,
  struct catnip_lua_widget_block* block,
  cairo_t* cr
);

#endif
