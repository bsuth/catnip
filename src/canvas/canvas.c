#include "canvas.h"
#include "server/display.h"
#include "server/scene.h"
#include <drm_fourcc.h>
#include <stdio.h>
#include <stdlib.h>
#include <wayland-server-core.h>
#include <wlr/interfaces/wlr_buffer.h>

// -----------------------------------------------------------------------------
// Canvas Buffer
// -----------------------------------------------------------------------------

static void
canvas_buffer_destroy(struct wlr_buffer* wlr_buffer)
{
}

static bool
canvas_buffer_begin_data_ptr_access(
  struct wlr_buffer* wlr_buffer,
  uint32_t flags,
  void** data,
  uint32_t* format,
  size_t* stride
)
{
  struct catnip_canvas* canvas = wl_container_of(wlr_buffer, canvas, buffer);

  *data = (void*) cairo_image_surface_get_data(canvas->cairo_surface);
  *format = DRM_FORMAT_ARGB8888;
  *stride = cairo_image_surface_get_stride(canvas->cairo_surface);

  return true;
}

static void
canvas_buffer_end_data_ptr_access(struct wlr_buffer* wlr_buffer)
{
}

static const struct wlr_buffer_impl canvas_buffer_impl = {
  .destroy = canvas_buffer_destroy,
  .begin_data_ptr_access = canvas_buffer_begin_data_ptr_access,
  .end_data_ptr_access = canvas_buffer_end_data_ptr_access,
};

// -----------------------------------------------------------------------------
// Canvas
// -----------------------------------------------------------------------------

struct catnip_canvas*
canvas_create(int width, int height)
{
  struct catnip_canvas* canvas = calloc(1, sizeof(struct catnip_canvas));

  canvas->cairo_surface =
    cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  canvas->cr = cairo_create(canvas->cairo_surface);

  wlr_buffer_init(&canvas->buffer, &canvas_buffer_impl, width, height);
  canvas->scene_tree = wlr_scene_tree_create(&server_scene->tree);
  canvas->scene_buffer =
    wlr_scene_buffer_create(canvas->scene_tree, &canvas->buffer);

  return canvas;
}

static void
canvas_refresh_task(void* data)
{
  struct catnip_canvas* canvas = data;

  if (canvas == NULL) {
    // Since the refresh task does not run until the next event loop tick, we
    // need to make sure that the canvas is still actually valid!
    return;
  }

  // TODO: be smarter about damaging? Not sure how beneficial it actually is.
  wlr_scene_buffer_set_buffer_with_damage(
    canvas->scene_buffer,
    &canvas->buffer,
    NULL
  );

  canvas->refresh_task = NULL;
}

void
canvas_refresh(struct catnip_canvas* canvas)
{
  if (canvas->refresh_task != NULL) {
    return; // task has already been queued
  }

  // Refreshing the canvas actually queues the buffer to be fully damaged in
  // the next event loop tick using Wayland's idle tasks.
  struct wl_event_loop* loop = wl_display_get_event_loop(server_display);
  canvas->refresh_task =
    wl_event_loop_add_idle(loop, canvas_refresh_task, canvas);
}

void
canvas_clear(struct catnip_canvas* canvas)
{
  cairo_save(canvas->cr);
  cairo_set_operator(canvas->cr, CAIRO_OPERATOR_CLEAR);
  cairo_paint(canvas->cr);
  cairo_restore(canvas->cr);
  canvas_refresh(canvas);
}

void
canvas_move(struct catnip_canvas* canvas, int new_x, int new_y)
{
  wlr_scene_node_set_position(&canvas->scene_tree->node, new_x, new_y);
}

void
canvas_resize(struct catnip_canvas* canvas, int new_width, int new_height)
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

  canvas_refresh(canvas);
}

void
canvas_destroy(struct catnip_canvas* canvas)
{
  cairo_surface_destroy(canvas->cairo_surface);
  cairo_destroy(canvas->cr);

  wlr_buffer_drop(&canvas->buffer);
  wlr_scene_node_destroy(&canvas->scene_buffer->node);
  wlr_scene_node_destroy(&canvas->scene_tree->node);

  free(canvas);
}

// -----------------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------------

int
canvas_get_x(struct catnip_canvas* canvas)
{
  return canvas->scene_tree->node.x;
}

int
canvas_get_y(struct catnip_canvas* canvas)
{
  return canvas->scene_tree->node.y;
}

int
canvas_get_width(struct catnip_canvas* canvas)
{
  return canvas->buffer.width;
}

int
canvas_get_height(struct catnip_canvas* canvas)
{
  return canvas->buffer.height;
}

bool
canvas_get_visible(struct catnip_canvas* canvas)
{
  return canvas->scene_tree->node.enabled;
}

// -----------------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------------

void
canvas_set_x(struct catnip_canvas* canvas, int new_x)
{
  canvas_move(canvas, new_x, canvas_get_y(canvas));
}

void
canvas_set_y(struct catnip_canvas* canvas, int new_y)
{
  canvas_move(canvas, canvas_get_x(canvas), new_y);
}

void
canvas_set_width(struct catnip_canvas* canvas, int new_width)
{
  canvas_resize(canvas, new_width, canvas_get_height(canvas));
}

void
canvas_set_height(struct catnip_canvas* canvas, int new_height)
{
  canvas_resize(canvas, canvas_get_width(canvas), new_height);
}

void
canvas_set_visible(struct catnip_canvas* canvas, bool new_visible)
{
  wlr_scene_node_set_enabled(&canvas->scene_tree->node, new_visible);
}
