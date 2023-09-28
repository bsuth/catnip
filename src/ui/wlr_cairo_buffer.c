#include "wlr_cairo_buffer.h"
#include <drm_fourcc.h>
#include <stdlib.h>
#include <wlr/interfaces/wlr_buffer.h>

static void
wlr_cairo_buffer_destroy(struct wlr_buffer* wlr_buffer)
{
  struct wlr_cairo_buffer* buffer = wl_container_of(wlr_buffer, buffer, base);
  free(buffer);
}

static bool
wlr_cairo_buffer_begin_data_ptr_access(
  struct wlr_buffer* wlr_buffer,
  uint32_t flags,
  void** data,
  uint32_t* format,
  size_t* stride
)
{
  struct wlr_cairo_buffer* buffer = wl_container_of(wlr_buffer, buffer, base);

  if (buffer->data == NULL) {
    return false;
  }

  *data = (void*) buffer->data;
  *format = buffer->format;
  *stride = buffer->stride;

  return true;
}

static void
wlr_cairo_buffer_end_data_ptr_access(struct wlr_buffer* wlr_buffer)
{
}

static const struct wlr_buffer_impl wlr_cairo_buffer_impl = {
  .destroy = wlr_cairo_buffer_destroy,
  .begin_data_ptr_access = wlr_cairo_buffer_begin_data_ptr_access,
  .end_data_ptr_access = wlr_cairo_buffer_end_data_ptr_access,
};

struct wlr_cairo_buffer*
create_wlr_cairo_buffer(cairo_surface_t* cairo_surface)
{
  struct wlr_cairo_buffer* buffer = malloc(sizeof(struct wlr_cairo_buffer));

  wlr_buffer_init(
    &buffer->base,
    &wlr_cairo_buffer_impl,
    cairo_image_surface_get_width(cairo_surface),
    cairo_image_surface_get_height(cairo_surface)
  );

  buffer->format = DRM_FORMAT_ARGB8888;
  buffer->stride = cairo_image_surface_get_stride(cairo_surface);
  buffer->data = cairo_image_surface_get_data(cairo_surface);

  return buffer;
}
