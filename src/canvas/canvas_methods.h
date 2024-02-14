#ifndef CATNIP_CANVAS_METHODS_H
#define CATNIP_CANVAS_METHODS_H

#include "canvas/canvas.h"

void
catnip_canvas_refresh(struct catnip_canvas* canvas);

void
catnip_canvas_clear(struct catnip_canvas* canvas);

void
catnip_canvas_move(struct catnip_canvas* canvas, int new_x, int new_y);

void
catnip_canvas_resize(
  struct catnip_canvas* canvas,
  int new_width,
  int new_height
);

#endif
