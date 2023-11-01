#include "shapes.h"
#include "utils/cairo.h"

void
canvas_rectangle(
  struct catnip_canvas* canvas,
  struct canvas_rectangle* rectangle
)
{
  cairo_t* cr = canvas->cr;
  cairo_save(cr);

  if (rectangle->bg != -1) {
    cairo_set_source_hexa(cr, rectangle->bg, rectangle->opacity);

    int x = rectangle->x + rectangle->border_width;
    int y = rectangle->y + rectangle->border_width;
    int width = rectangle->width - 2 * rectangle->border_width;
    int height = rectangle->height - 2 * rectangle->border_width;

    rectangle->radius == 0
      ? cairo_rectangle(cr, x, y, width, height)
      : cairo_rounded_rectangle(cr, x, y, width, height, rectangle->radius);

    cairo_fill(cr);
  }

  if (rectangle->border != -1) {
    cairo_set_line_width(cr, rectangle->border_width);
    cairo_set_source_hexa(cr, rectangle->border, rectangle->opacity);

    int x = rectangle->x + (double) rectangle->border_width / 2;
    int y = rectangle->y + (double) rectangle->border_width / 2;
    int width = rectangle->width - rectangle->border_width;
    int height = rectangle->height - rectangle->border_width;
    int radius = rectangle->radius + rectangle->border_width;

    rectangle->radius == 0
      ? cairo_rectangle(cr, x, y, width, height)
      : cairo_rounded_rectangle(cr, x, y, width, height, radius);

    cairo_stroke(cr);
  }

  cairo_restore(cr);
  canvas_refresh(canvas);
}
