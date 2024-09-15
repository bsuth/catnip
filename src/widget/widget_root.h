#ifndef CATNIP_WIDGET_ROOT_H
#define CATNIP_WIDGET_ROOT_H

#include "extensions/lua.h"
#include "widget/widget_base.h"
#include <cairo.h>
#include <wlr/types/wlr_buffer.h>
#include <wlr/types/wlr_scene.h>

struct catnip_widget_root {
  lua_Ref lua_widget;
  struct catnip_widget_base* widget;

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
    bool render;
  } request;
};

struct catnip_widget_root*
catnip_widget_root_create(int initial_width, int initial_height);

void
catnip_widget_root_destroy(struct catnip_widget_root* root);

void
catnip_widget_root_request_resize(
  struct catnip_widget_root* root,
  int new_width,
  int new_height
);

void
catnip_widget_root_request_render(struct catnip_widget_root* root);

#endif
