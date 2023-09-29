#ifndef CATNIP_UI_DRAWABLE_H
#define CATNIP_UI_DRAWABLE_H

#include <cairo.h>
#include <wlr/types/wlr_scene.h>

struct drawable {
  struct wlr_buffer buffer;
  struct wlr_scene_tree* scene_tree;
  struct wlr_scene_buffer* scene_buffer;
  cairo_surface_t* surface;
  cairo_t* cr;
};

struct drawable*
create_drawable(int width, int height);

void
drawable_refresh(struct drawable* drawable);

void
drawable_set_width(struct drawable* drawable, int new_width);

void
drawable_set_height(struct drawable* drawable, int new_height);

#endif
