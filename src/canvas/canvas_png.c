#include "canvas_png.h"
#include "utils/log.h"
#include <stdlib.h>
#include <string.h>

struct catnip_canvas_png*
catnip_canvas_png_create(const char* path)
{
  struct catnip_canvas_png* png = calloc(1, sizeof(struct catnip_canvas_png));

  png->path = strdup(path);
  png->surface = cairo_image_surface_create_from_png(png->path);

  if (cairo_surface_status(png->surface) == CAIRO_STATUS_SUCCESS) {
    png->width = cairo_image_surface_get_width(png->surface);
    png->height = cairo_image_surface_get_height(png->surface);
  } else {
    png->surface = NULL;
    log_warning("failed to load png: %s", path);
  }

  return png;
}

void
catnip_canvas_png_destroy(struct catnip_canvas_png* png)
{
  cairo_surface_destroy(png->surface);
  free(png->path);
  free(png);
}
