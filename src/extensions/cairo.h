#ifndef CATNIP_EXTENSIONS_CAIRO_H
#define CATNIP_EXTENSIONS_CAIRO_H

#include <cairo.h>

void
cairo_set_source_hex(cairo_t* cr, int hex);

void
cairo_set_source_hexa(cairo_t* cr, int hex, double alpha);

void
cairo_rounded_rectangle(
  cairo_t* cr,
  double x,
  double y,
  double width,
  double height,
  double radius_top_left,
  double radius_top_right,
  double radius_bottom_right,
  double radius_bottom_left
);

#endif
