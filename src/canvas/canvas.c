#include "canvas.h"
#include "canvas/canvas_buffer.h"
#include "canvas/canvas_cache.h"
#include "compositor/event_loop.h"
#include "compositor/scene.h"
#include <stdlib.h>

void
catnip_canvas_setup(struct catnip_canvas* canvas, int width, int height)
{
  canvas->cairo.surface =
    cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  canvas->cairo.cr = cairo_create(canvas->cairo.surface);

  wlr_buffer_init(&canvas->wlr.buffer, &catnip_canvas_buffer, width, height);
  canvas->wlr.scene_buffer =
    wlr_scene_buffer_create(&catnip_scene->tree, &canvas->wlr.buffer);

  canvas->cache = catnip_canvas_cache_create();
  canvas->event_sources.refresh = NULL;
}

void
catnip_canvas_destroy(struct catnip_canvas* canvas)
{
  cairo_surface_destroy(canvas->cairo.surface);
  cairo_destroy(canvas->cairo.cr);

  wlr_buffer_drop(&canvas->wlr.buffer);
  wlr_scene_node_destroy(&canvas->wlr.scene_buffer->node);

  catnip_canvas_cache_destroy(canvas->cache);

  if (canvas->event_sources.refresh != NULL) {
    wl_event_source_remove(canvas->event_sources.refresh);
  }
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
    canvas->wlr.scene_buffer,
    &canvas->wlr.buffer,
    NULL
  );

  canvas->event_sources.refresh = NULL;
}

void
catnip_canvas_refresh(struct catnip_canvas* canvas)
{
  if (canvas->event_sources.refresh != NULL) {
    return; // already queued
  }

  canvas->event_sources.refresh =
    wl_event_loop_add_idle(catnip_event_loop, __catnip_canvas_refresh, canvas);
}

void
catnip_canvas_resize(
  struct catnip_canvas* canvas,
  int new_width,
  int new_height
)
{
  if (new_width == canvas->wlr.buffer.width
      && new_height == canvas->wlr.buffer.height) {
    return; // nothing to do
  }

  cairo_surface_destroy(canvas->cairo.surface);
  cairo_destroy(canvas->cairo.cr);

  // Cairo does not allow an image surface to be resized after creation. Thus,
  // we have to destroy any previous surfaces / contexts and create new ones.
  canvas->cairo.surface =
    cairo_image_surface_create(CAIRO_FORMAT_ARGB32, new_width, new_height);
  canvas->cairo.cr = cairo_create(canvas->cairo.surface);

  canvas->wlr.buffer.width = new_width;
  canvas->wlr.buffer.height = new_height;

  // Unset buffer to force full update on refresh. Required to make sure that
  // canvas->scene_buffer->node.visible is updated properly.
  wlr_scene_buffer_set_buffer_with_damage(canvas->wlr.scene_buffer, NULL, NULL);

  catnip_canvas_refresh(canvas);
}

void
catnip_canvas_clear(struct catnip_canvas* canvas)
{
  catnip_canvas_cache_clear(canvas->cache);
  cairo_save(canvas->cairo.cr);
  cairo_set_operator(canvas->cairo.cr, CAIRO_OPERATOR_CLEAR);
  cairo_paint(canvas->cairo.cr);
  cairo_restore(canvas->cairo.cr);
  catnip_canvas_refresh(canvas);
  catnip_canvas_cache_reset(canvas->cache);
}
