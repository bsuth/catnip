#ifndef CATNIP_UI_WLR_CAIRO_BUFFER_H
#define CATNIP_UI_WLR_CAIRO_BUFFER_H

#include <cairo.h>
#include <wlr/types/wlr_buffer.h>

struct wlr_cairo_buffer {
  struct wlr_buffer base;
  const void* data;
  uint32_t format;
  size_t stride;
};

struct wlr_cairo_buffer*
create_wlr_cairo_buffer(cairo_surface_t* cairo_surface);

#endif
