#ifndef CATNIP_CANVAS_TEXT_H
#define CATNIP_CANVAS_TEXT_H

#include "canvas/canvas.h"
#include "pango/pango-layout.h"
#include "pango/pango.h"

struct canvas_text {
  const char* markup;
  int x;
  int y;
  int width;
  int height;
  PangoAlignment alignment;
  PangoEllipsizeMode ellipsize;
  PangoWrapMode wrap;
};

void
canvas_text(struct catnip_canvas* canvas, struct canvas_text* text);

#endif
