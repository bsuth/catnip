#ifndef CATNIP_CANVAS_SVG_H
#define CATNIP_CANVAS_SVG_H

#include "canvas/canvas.h"
#include <librsvg/rsvg.h>

struct canvas_svg {
  RsvgHandle* rsvg;
  int x;
  int y;
  int width;
  int height;
};

void
canvas_svg(struct catnip_canvas* canvas, struct canvas_svg* svg);

#endif
