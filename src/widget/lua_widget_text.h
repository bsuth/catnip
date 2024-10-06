#ifndef CATNIP_LUA_WIDGET_TEXT_H
#define CATNIP_LUA_WIDGET_TEXT_H

#include "widget/lua_widget_base.h"
#include <cairo.h>
#include <lua.h>

struct catnip_lua_widget_text {
  // IMPORTANT: Keep `base` at the start to allow pointer casting.
  struct catnip_lua_widget_base base;

  struct {
  } styles;

  struct {
  } computed;
};

void
catnip_lua_widget_text_init(lua_State* L);

int
catnip_lua_widget_lua_text(lua_State* L);

void
catnip_lua_widget_text_draw(
  lua_State* L,
  struct catnip_lua_widget_text* text,
  cairo_t* cr
);

#endif
