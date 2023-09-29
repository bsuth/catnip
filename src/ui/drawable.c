#include "drawable.h"
#include "pixman.h"
#include "server/scene.h"
#include <drm_fourcc.h>
#include <stdio.h>
#include <stdlib.h>
#include <wlr/interfaces/wlr_buffer.h>

// -----------------------------------------------------------------------------
// Drawable Buffer
// -----------------------------------------------------------------------------

static void
drawable_buffer_destroy(struct wlr_buffer* wlr_buffer)
{
  struct drawable* drawable = wl_container_of(wlr_buffer, drawable, buffer);
  free(drawable);
}

static bool
drawable_buffer_begin_data_ptr_access(
  struct wlr_buffer* wlr_buffer,
  uint32_t flags,
  void** data,
  uint32_t* format,
  size_t* stride
)
{
  struct drawable* drawable = wl_container_of(wlr_buffer, drawable, buffer);

  *data = (void*) cairo_image_surface_get_data(drawable->surface);
  *format = DRM_FORMAT_ARGB8888;
  *stride = cairo_image_surface_get_stride(drawable->surface);

  return true;
}

static void
drawable_buffer_end_data_ptr_access(struct wlr_buffer* wlr_buffer)
{
}

static const struct wlr_buffer_impl drawable_buffer_impl = {
  .destroy = drawable_buffer_destroy,
  .begin_data_ptr_access = drawable_buffer_begin_data_ptr_access,
  .end_data_ptr_access = drawable_buffer_end_data_ptr_access,
};

// -----------------------------------------------------------------------------
// Drawable
// -----------------------------------------------------------------------------

struct drawable*
create_drawable(int width, int height)
{
  struct drawable* drawable = malloc(sizeof(struct drawable));

  drawable->surface =
    cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  drawable->cr = cairo_create(drawable->surface);

  wlr_buffer_init(&drawable->buffer, &drawable_buffer_impl, width, height);

  drawable->scene_tree = wlr_scene_tree_create(&server_scene->tree);
  drawable->scene_buffer =
    wlr_scene_buffer_create(drawable->scene_tree, &drawable->buffer);

  return drawable;
}

void
drawable_refresh(struct drawable* drawable)
{
  // Unset buffer to force full update on next call. Required to make sure that
  // drawable->scene_buffer->node.visible is updated properly.
  wlr_scene_buffer_set_buffer_with_damage(drawable->scene_buffer, NULL, NULL);
  wlr_scene_buffer_set_buffer_with_damage(
    drawable->scene_buffer,
    &drawable->buffer,
    NULL
  );
}

// -----------------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------------

static void
drawable_resize(struct drawable* drawable, int new_width, int new_height)
{
  if (drawable->surface != NULL) {
    cairo_surface_destroy(drawable->surface);
  }

  if (drawable->cr != NULL) {
    cairo_destroy(drawable->cr);
  }

  drawable->surface =
    cairo_image_surface_create(CAIRO_FORMAT_ARGB32, new_width, new_height);
  drawable->cr = cairo_create(drawable->surface);

  drawable->buffer.width = new_width;
  drawable->buffer.height = new_height;

  drawable_refresh(drawable);
}

void
drawable_set_width(struct drawable* drawable, int new_width)
{
  drawable_resize(drawable, new_width, drawable->buffer.height);
}

void
drawable_set_height(struct drawable* drawable, int new_height)
{
  drawable_resize(drawable, drawable->buffer.width, new_height);
}
