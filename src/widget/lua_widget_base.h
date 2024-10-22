#ifndef CATNIP_LUA_WIDGET_BASE_H
#define CATNIP_LUA_WIDGET_BASE_H

#include <lua.h>
#include <stdbool.h>

enum catnip_lua_widget_type {
  CATNIP_LUA_WIDGET_BLOCK,
  CATNIP_LUA_WIDGET_NONE,
  CATNIP_LUA_WIDGET_PNG,
  CATNIP_LUA_WIDGET_ROOT,
  CATNIP_LUA_WIDGET_SVG,
  CATNIP_LUA_WIDGET_TEXT,
};

struct catnip_lua_widget_base {
  enum catnip_lua_widget_type type;
  struct catnip_lua_widget_base* parent;

  struct {
    struct wl_event_source* request_layout;
    struct wl_event_source* request_draw;
  } event_sources;

  struct {
    int x;
    int y;
    int width;
    int height;
  } bounding_box;
};

void
catnip_lua_widget_base_setup(lua_State* L, struct catnip_lua_widget_base* base);

void
catnip_lua_widget_base_cleanup(
  lua_State* L,
  struct catnip_lua_widget_base* base
);

enum catnip_lua_widget_type
catnip_lua_widget_base_type(lua_State* L, int idx);

void
catnip_lua_widget_base_request_layout(
  lua_State* L,
  struct catnip_lua_widget_base* base
);

void
catnip_lua_widget_base_request_draw(
  lua_State* L,
  struct catnip_lua_widget_base* base
);

#endif
