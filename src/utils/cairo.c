#include "cairo.h"
#include <math.h>

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
  double radius_top_left,
  double radius_top_right,
  double radius_bottom_right,
  double radius_bottom_left
)
{
  double half_width = width / 2;
  double half_height = height / 2;
  double max_radius = half_width > half_height ? half_width : half_height;

  radius_top_left = radius_top_left > max_radius ? max_radius : radius_top_left;
  radius_top_right =
    radius_top_right > max_radius ? max_radius : radius_top_right;
  radius_bottom_right =
    radius_bottom_right > max_radius ? max_radius : radius_bottom_right;
  radius_bottom_left =
    radius_bottom_left > max_radius ? max_radius : radius_bottom_left;

  cairo_new_sub_path(cr);

  cairo_arc(
    cr,
    x + radius_top_left,
    y + radius_top_left,
    radius_top_left,
    M_PI,
    3 * M_PI_2
  );

  cairo_arc(
    cr,
    x + width - radius_top_right,
    y + radius_top_right,
    radius_top_right,
    -M_PI_2,
    0
  );

  cairo_arc(
    cr,
    x + width - radius_bottom_right,
    y + height - radius_bottom_right,
    radius_bottom_right,
    0,
    M_PI_2
  );

  cairo_arc(
    cr,
    x + radius_bottom_left,
    y + height - radius_bottom_left,
    radius_bottom_left,
    M_PI_2,
    M_PI
  );

  cairo_close_path(cr);
}
