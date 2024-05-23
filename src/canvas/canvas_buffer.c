#include "canvas_buffer.h"
#include "canvas/canvas.h"
#include <drm_fourcc.h>

static void
catnip_canvas_buffer_destroy(struct wlr_buffer* wlr_buffer)
{
}

static bool
catnip_canvas_buffer_begin_data_ptr_access(
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
catnip_canvas_buffer_end_data_ptr_access(struct wlr_buffer* wlr_buffer)
{
}

const struct wlr_buffer_impl catnip_canvas_buffer = {
  .destroy = catnip_canvas_buffer_destroy,
  .begin_data_ptr_access = catnip_canvas_buffer_begin_data_ptr_access,
  .end_data_ptr_access = catnip_canvas_buffer_end_data_ptr_access,
};
