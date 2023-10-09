#include "box.h"
#include "ui/root.h"
#include <stdio.h>
#include <stdlib.h>

struct ui_box*
create_ui_box()
{
  struct ui_box* box = malloc(sizeof(struct ui_box));

  box->styles.background = -1;

  return box;
}

void
resolve_ui_box(struct ui_box* box)
{
  // TODO: resolve widths, heights, etc
}

void
render_ui_box(struct ui_box* box)
{
  // cairo_t* cr = box->root->cr;

  // if (box->styles.background > 0) {
  //   int bg = box->styles.background;

  //   double r = ((double) ((bg & 0xFF0000) >> 16)) / 0xFF;
  //   double g = ((double) ((bg & 0x00FF00) >> 8)) / 0xFF;
  //   double b = ((double) (bg & 0x0000FF)) / 0xFF;

  //   cairo_set_source_rgb(cr, r, g, b);
  //   cairo_rectangle(cr, 0, 0, ui_box_get_width(box), ui_box_get_height(box));
  //   cairo_fill(cr);
  // }

  // ui_root_refresh(box->root);
}

// -----------------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------------

int
ui_box_get_width(struct ui_box* box)
{
  // return box->root->buffer.width;
  return 0;
}

int
ui_box_get_height(struct ui_box* box)
{
  // return box->root->buffer.height;
  return 0;
}

int
ui_box_get_background(struct ui_box* box)
{
  return box->styles.background;
}

// -----------------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------------

void
ui_box_set_width(
  struct ui_box* box,
  int new_width,
  enum UI_SIZE_TYPE new_width_type
)
{
  box->styles.width = new_width;
  box->styles.width_type = new_width_type;

  if (new_width_type == UI_SIZE_PX) {
    // ui_root_set_width(box->root, new_width);
  } else {
    resolve_ui_box(box);
  }

  render_ui_box(box);
}

void
ui_box_set_height(
  struct ui_box* box,
  int new_height,
  enum UI_SIZE_TYPE new_height_type
)
{
  box->styles.height = new_height;
  box->styles.height_type = new_height_type;

  if (new_height_type == UI_SIZE_PX) {
    // ui_root_set_height(box->root, new_height);
  } else {
    resolve_ui_box(box);
  }

  render_ui_box(box);
}

void
ui_box_set_background(struct ui_box* box, int new_background)
{
  box->styles.background = new_background;
  render_ui_box(box);
}
