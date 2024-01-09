#include "canvas.h"
#include "canvas/canvas_buffer.h"
#include "events/event_loop.h"
#include "scene.h"
#include <stdlib.h>
#include <wlr/interfaces/wlr_buffer.h>

struct catnip_canvas*
catnip_canvas_create(int width, int height)
{
  struct catnip_canvas* canvas = calloc(1, sizeof(struct catnip_canvas));

  canvas->cairo_surface =
    cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  canvas->cr = cairo_create(canvas->cairo_surface);

  wlr_buffer_init(&canvas->buffer, &catnip_canvas_buffer, width, height);
  canvas->scene_tree = wlr_scene_tree_create(&catnip_scene->tree);
  canvas->scene_buffer =
    wlr_scene_buffer_create(canvas->scene_tree, &canvas->buffer);

  return canvas;
}

void
catnip_canvas_destroy(struct catnip_canvas* canvas)
{
  cairo_surface_destroy(canvas->cairo_surface);
  cairo_destroy(canvas->cr);

  wlr_buffer_drop(&canvas->buffer);
  wlr_scene_node_destroy(&canvas->scene_buffer->node);
  wlr_scene_node_destroy(&canvas->scene_tree->node);

  if (canvas->refresh_task != NULL) {
    wl_event_source_remove(canvas->refresh_task);
  }

  free(canvas);
}

static void
__catnip_canvas_refresh(void* data)
{
  struct catnip_canvas* canvas = data;

  if (canvas == NULL) {
    // Since the refresh task does not run until the next event loop tick, we
    // need to make sure that the canvas is still actually valid!
    return;
  }

  wlr_scene_buffer_set_buffer_with_damage(
    canvas->scene_buffer,
    &canvas->buffer,
    NULL
  );

  canvas->refresh_task = NULL;
}

void
catnip_canvas_refresh(struct catnip_canvas* canvas)
{
  if (canvas->refresh_task != NULL) {
    return; // already queued
  }

  canvas->refresh_task =
    catnip_event_loop_once(__catnip_canvas_refresh, canvas);
}

void
catnip_canvas_clear(struct catnip_canvas* canvas)
{
  cairo_save(canvas->cr);
  cairo_set_operator(canvas->cr, CAIRO_OPERATOR_CLEAR);
  cairo_paint(canvas->cr);
  cairo_restore(canvas->cr);
  catnip_canvas_refresh(canvas);
}

void
catnip_canvas_move(struct catnip_canvas* canvas, int new_x, int new_y)
{
  wlr_scene_node_set_position(&canvas->scene_tree->node, new_x, new_y);
}

void
catnip_canvas_resize(
  struct catnip_canvas* canvas,
  int new_width,
  int new_height
)
{
  if (new_width == canvas->buffer.width && new_height == canvas->buffer.height) {
    return; // nothing to do
  }

  cairo_surface_destroy(canvas->cairo_surface);
  cairo_destroy(canvas->cr);

  // Cairo does not allow an image surface to be resized after creation. Thus,
  // we have to destroy any previous surfaces / contexts and create new ones.
  canvas->cairo_surface =
    cairo_image_surface_create(CAIRO_FORMAT_ARGB32, new_width, new_height);
  canvas->cr = cairo_create(canvas->cairo_surface);

  canvas->buffer.width = new_width;
  canvas->buffer.height = new_height;

  // Unset buffer to force full update on refresh. Required to make sure that
  // canvas->scene_buffer->node.visible is updated properly.
  wlr_scene_buffer_set_buffer_with_damage(canvas->scene_buffer, NULL, NULL);

  catnip_canvas_refresh(canvas);
}
