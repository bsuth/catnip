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
};

// -----------------------------------------------------------------------------
// Canvas
// -----------------------------------------------------------------------------

struct catnip_canvas*
canvas_create(int width, int height);

void
canvas_move(struct catnip_canvas* canvas, int new_x, int new_y);

void
canvas_resize(struct catnip_canvas* canvas, int new_width, int new_height);

void
canvas_destroy(struct catnip_canvas* canvas);

// -----------------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------------

int
canvas_get_x(struct catnip_canvas* canvas);

int
canvas_get_y(struct catnip_canvas* canvas);

int
canvas_get_width(struct catnip_canvas* canvas);

int
canvas_get_height(struct catnip_canvas* canvas);

bool
canvas_get_visible(struct catnip_canvas* canvas);

// -----------------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------------

void
canvas_set_x(struct catnip_canvas* canvas, int new_x);

void
canvas_set_y(struct catnip_canvas* canvas, int new_y);

void
canvas_set_width(struct catnip_canvas* canvas, int new_width);

void
canvas_set_height(struct catnip_canvas* canvas, int new_height);

void
canvas_set_visible(struct catnip_canvas* canvas, bool new_visible);

#endif
