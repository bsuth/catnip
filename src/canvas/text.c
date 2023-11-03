#include "text.h"
#include "utils/cairo.h"
#include <pango/pangocairo.h>

void
canvas_text(struct catnip_canvas* canvas, struct canvas_text* text)
{
  PangoLayout* layout = pango_cairo_create_layout(canvas->cr);

  pango_layout_set_width(layout, text->width);
  pango_layout_set_height(layout, text->height);
  pango_layout_set_markup(layout, text->markup, -1);
  pango_layout_set_alignment(layout, text->alignment);
  pango_layout_set_ellipsize(layout, text->ellipsize);
  pango_layout_set_wrap(layout, text->wrap);

  cairo_save(canvas->cr);
  cairo_move_to(canvas->cr, text->x, text->y);
  pango_cairo_show_layout(canvas->cr, layout);
  cairo_restore(canvas->cr);

  canvas_refresh(canvas);
}
