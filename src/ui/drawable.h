#ifndef CATNIP_UI_DRAWABLE_H
#define CATNIP_UI_DRAWABLE_H

#include "ui/wlr_cairo_buffer.h"
#include <cairo.h>
#include <wlr/types/wlr_scene.h>

struct drawable {
  struct wlr_scene_tree* scene_tree;
  struct wlr_cairo_buffer* buffer;
  cairo_surface_t* cairo_surface;
  cairo_t* cr;
};

struct drawable*
create_drawable();

#endif
