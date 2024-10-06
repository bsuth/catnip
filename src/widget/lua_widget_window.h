#ifndef CATNIP_LUA_WIDGET_WINDOW_H
#define CATNIP_LUA_WIDGET_WINDOW_H

#include "widget/lua_widget_base.h"
#include <cairo.h>
#include <lua.h>

struct catnip_lua_widget_window {
  // IMPORTANT: Keep `base` at the start to allow pointer casting.
  struct catnip_lua_widget_base base;

  struct {
  } styles;

  struct {
  } computed;
};

void
catnip_lua_widget_window_init(lua_State* L);

int
catnip_lua_widget_lua_window(lua_State* L);

void
catnip_lua_widget_window_draw(
  lua_State* L,
  struct catnip_lua_widget_window* window,
  cairo_t* cr
);

#endif
