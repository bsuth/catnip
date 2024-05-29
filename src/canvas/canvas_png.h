#ifndef CATNIP_CANVAS_PNG_H
#define CATNIP_CANVAS_PNG_H

#include <cairo.h>
#include <lua.h>

struct catnip_canvas_png {
  char* path;
  cairo_surface_t* surface;
  double width;
  double height;
};

struct catnip_canvas_png*
catnip_canvas_png_create(const char* path);

void
catnip_canvas_png_destroy(struct catnip_canvas_png* png);

#endif
