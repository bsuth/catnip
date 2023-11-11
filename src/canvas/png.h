#ifndef CATNIP_CANVAS_PNG_H
#define CATNIP_CANVAS_PNG_H

#include "canvas/canvas.h"

struct canvas_png {
  cairo_surface_t* cairo_surface;
  int x;
  int y;
  int width;
  int height;
};

void
canvas_png(struct catnip_canvas* canvas, struct canvas_png* png);

#endif
