#include "canvas_methods.h"
#include "event_loop.h"

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
    wl_event_loop_add_idle(catnip_event_loop, __catnip_canvas_refresh, canvas);
}

void
catnip_canvas_resize(
  struct catnip_canvas* canvas,
  int new_width,
  int new_height
)
{
  if (new_width == canvas->buffer.width
      && new_height == canvas->buffer.height) {
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

void
catnip_canvas_clear(struct catnip_canvas* canvas)
{
  cairo_save(canvas->cr);
  cairo_set_operator(canvas->cr, CAIRO_OPERATOR_CLEAR);
  cairo_paint(canvas->cr);
  cairo_restore(canvas->cr);
  catnip_canvas_refresh(canvas);
}
