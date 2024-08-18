#ifndef CATNIP_CANVAS_H
#define CATNIP_CANVAS_H

#include <cairo.h>
#include <wlr/types/wlr_buffer.h>
#include <wlr/types/wlr_scene.h>

struct catnip_canvas {
  struct catnip_canvas_cache* cache;

  struct {
    struct wlr_buffer buffer;
    struct wlr_scene_buffer* scene_buffer;
  } wlr;

  struct {
    cairo_surface_t* surface;
    cairo_t* cr;
  } cairo;

  struct {
    // Use a flag to indicate whether the canvas needs to refresh (i.e. by
    // damaging the entire buffer). Using a flag allows us to manipulate the
    // canvas buffer as much as we want before actually telling Wayland to
    // update the scene.
    struct wl_event_source* refresh;
  } event_sources;
};

void
catnip_canvas_setup(struct catnip_canvas* canvas, int width, int height);

void
catnip_canvas_destroy(struct catnip_canvas* canvas);

void
catnip_canvas_refresh(struct catnip_canvas* canvas);

void
catnip_canvas_resize(
  struct catnip_canvas* canvas,
  int new_width,
  int new_height
);

void
catnip_canvas_clear(struct catnip_canvas* canvas);

#endif
