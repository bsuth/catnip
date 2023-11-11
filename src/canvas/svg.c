#include "svg.h"
#include "utils/log.h"
#include <librsvg/rsvg.h>

void
canvas_svg(struct catnip_canvas* canvas, struct canvas_svg* svg)
{
  RsvgRectangle viewport = {
    .x = svg->x,
    .y = svg->y,
    .width = svg->width,
    .height = svg->height,
  };

  if (viewport.width == -1 || viewport.height == -1) {
    double width_px, height_px;
    rsvg_handle_get_intrinsic_size_in_pixels(svg->rsvg, &width_px, &height_px);

    if (viewport.width == -1 && viewport.height == -1) {
      viewport.width = width_px;
      viewport.height = height_px;
    } else if (viewport.width == -1) {
      viewport.width = viewport.height * (width_px / height_px);
    } else {
      viewport.height = viewport.width * (width_px / height_px);
    }
  }

  rsvg_handle_render_document(svg->rsvg, canvas->cr, &viewport, NULL);
  canvas_refresh(canvas);
}
