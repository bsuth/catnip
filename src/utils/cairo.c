#include "cairo.h"
#include "math.h"

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

void
cairo_rounded_rectangle(
  cairo_t* cr,
  double x,
  double y,
  double width,
  double height,
  double radius
)
{
  cairo_new_sub_path(cr);
  cairo_arc(cr, x + radius, y + radius, radius, M_PI, 3 * M_PI_2);
  cairo_arc(cr, x + width - radius, y + radius, radius, -M_PI_2, 0);
  cairo_arc(cr, x + width - radius, y + height - radius, radius, 0, M_PI_2);
  cairo_arc(cr, x + radius, y + height - radius, radius, M_PI_2, M_PI);
  cairo_close_path(cr);
}
