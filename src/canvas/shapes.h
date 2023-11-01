#ifndef CATNIP_CANVAS_SHAPES_H
#define CATNIP_CANVAS_SHAPES_H

#include "canvas/canvas.h"

struct canvas_rectangle {
  int x;
  int y;
  int width;
  int height;
  int bg;
  int border;
  int border_width;
  int radius;
  double opacity;
};

void
canvas_rectangle(
  struct catnip_canvas* canvas,
  struct canvas_rectangle* rectangle
);

#endif
