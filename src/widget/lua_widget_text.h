#ifndef CATNIP_LUA_WIDGET_TEXT_H
#define CATNIP_LUA_WIDGET_TEXT_H

#include "extensions/lua.h"
#include "widget/lua_widget_base.h"
#include <cairo.h>
#include <lua.h>
#include <pango/pango-layout.h>

struct catnip_lua_widget_text {
  // IMPORTANT: Keep `base` at the start to allow pointer casting.
  struct catnip_lua_widget_base base;

  PangoContext* context;
  PangoLayout* layout;
  PangoAttrList* attributes;
  bool refresh_attributes;

  struct {
    const char* text;
    lua_Ref text_ref;
    int size;
    int weight;
    bool italic;
    int color;
    double opacity;
    const char* font;
    lua_Ref font_ref;
    PangoAlignment halign;
    lua_Ref halign_ref;
    PangoAlignment valign;
    lua_Ref valign_ref;
    PangoEllipsizeMode ellipsis;
    lua_Ref ellipsis_ref;
    PangoWrapMode wrap;
    lua_Ref wrap_ref;
  } styles;
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
