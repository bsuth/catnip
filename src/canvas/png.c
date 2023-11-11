#include "png.h"

void
canvas_png(struct catnip_canvas* canvas, struct canvas_png* png)
{
  cairo_save(canvas->cr);

  int surface_width = cairo_image_surface_get_width(png->cairo_surface);
  int surface_height = cairo_image_surface_get_height(png->cairo_surface);

  if (png->width != -1 || png->height != -1) {
    double scale_x = (double) png->width / surface_width;
    double scale_y = (double) png->height / surface_height;

    cairo_scale(
      canvas->cr,
      png->width != -1 ? scale_x : scale_y,
      png->height != -1 ? scale_y : scale_x
    );
  }

  cairo_set_source_surface(canvas->cr, png->cairo_surface, png->x, png->y);
  cairo_rectangle(canvas->cr, 0, 0, surface_width, surface_height);
  cairo_fill(canvas->cr);

  cairo_restore(canvas->cr);
  canvas_refresh(canvas);
}
