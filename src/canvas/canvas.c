#include "canvas.h"
#include "canvas/canvas_buffer.h"
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
