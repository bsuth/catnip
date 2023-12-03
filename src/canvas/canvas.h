#ifndef CATNIP_CANVAS_H
#define CATNIP_CANVAS_H

#include <cairo.h>
#include <wlr/types/wlr_scene.h>

struct catnip_canvas {
  struct wlr_buffer buffer;
  struct wlr_scene_tree* scene_tree;
  struct wlr_scene_buffer* scene_buffer;

  cairo_surface_t* cairo_surface;
  cairo_t* cr;

  // TODO: update this message
  // Use a flag to indicate whether the canvas needs to refresh (i.e. by
  // damaging the entire buffer). Using a flag allows us to manipulate the
  // canvas buffer as much as we want before actually telling Wayland to update
  // the scene.
  struct wl_event_source* refresh_task;
};

// -----------------------------------------------------------------------------
// Canvas
// -----------------------------------------------------------------------------

struct catnip_canvas*
catnip_canvas_create(int width, int height);

void
catnip_canvas_refresh(struct catnip_canvas* canvas);

void
catnip_canvas_clear(struct catnip_canvas* canvas);

void
catnip_canvas_move(struct catnip_canvas* canvas, int new_x, int new_y);

void
catnip_canvas_resize(
  struct catnip_canvas* canvas,
  int new_width,
  int new_height
);

void
catnip_canvas_destroy(struct catnip_canvas* canvas);

#endif
