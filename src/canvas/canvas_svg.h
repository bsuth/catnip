#ifndef CATNIP_CANVAS_SVG_H
#define CATNIP_CANVAS_SVG_H

#include <librsvg/rsvg.h>

struct catnip_canvas_svg {
  char* document;
  char* styles;
  RsvgHandle* rsvg;
  double width;
  double height;
};

struct catnip_canvas_svg*
catnip_canvas_svg_create(const char* document, const char* styles);

void
catnip_canvas_svg_destroy(struct catnip_canvas_svg* svg);

#endif
