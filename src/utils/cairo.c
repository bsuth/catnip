#include "cairo.h"

void
cairo_set_source_hex(cairo_t* cr, int hex)
{
  cairo_set_source_rgb(
    cr,
    ((double) ((hex & 0xFF0000) >> 16)) / 0xFF,
    ((double) ((hex & 0x00FF00) >> 8)) / 0xFF,
    ((double) (hex & 0x0000FF)) / 0xFF
  );
}

void
cairo_set_source_hexa(cairo_t* cr, int hex, double alpha)
{
  cairo_set_source_rgba(
    cr,
    ((double) ((hex & 0xFF0000) >> 16)) / 0xFF,
    ((double) ((hex & 0x00FF00) >> 8)) / 0xFF,
    ((double) (hex & 0x0000FF)) / 0xFF,
    alpha
  );
}
