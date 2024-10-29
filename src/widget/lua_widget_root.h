#ifndef CATNIP_LUA_WIDGET_ROOT_H
#define CATNIP_LUA_WIDGET_ROOT_H

#include "extensions/lua.h"
#include "widget/lua_widget_base.h"
#include <cairo.h>
#include <wlr/types/wlr_buffer.h>
#include <wlr/types/wlr_scene.h>

struct catnip_lua_widget_root {
  // IMPORTANT: Keep `base` at the start to allow pointer casting.
  struct catnip_lua_widget_base base;

  struct catnip_lua_widget_block* block;
  lua_Ref block_ref;

  struct {
    struct wlr_buffer buffer;
    struct wlr_scene_buffer* scene_buffer;
  } wlr;

  struct {
    cairo_surface_t* surface;
    cairo_t* cr;
  } cairo;

  struct {
    struct wl_event_source* event_source;
    int width;
    int height;
    bool layout;
    bool draw;
  } request;
};

void
catnip_lua_widget_root_init(lua_State* L);

int
catnip_lua_widget_lua_root(lua_State* L);

void
catnip_lua_widget_root_request_resize(
  struct catnip_lua_widget_root* root,
  int new_width,
  int new_height
);

void
catnip_lua_widget_root_request_layout(struct catnip_lua_widget_root* root);

void
catnip_lua_widget_root_request_draw(struct catnip_lua_widget_root* root);

#endif
