#ifndef CATNIP_UI_BOX_H
#define CATNIP_UI_BOX_H

#include "ui/types.h"

struct ui_box_properties {};

struct ui_box_styles {
  int width;
  enum UI_SIZE_TYPE width_type;
  int height;
  enum UI_SIZE_TYPE height_type;
  int background;
};

struct ui_box {
  struct drawable* drawable;
  struct ui_box_properties properties;
  struct ui_box_styles styles;
};

struct ui_box*
create_ui_box();

void
render_ui_box(struct ui_box* box);

// -----------------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------------

int
ui_box_get_width(struct ui_box* box);

int
ui_box_get_height(struct ui_box* box);

int
ui_box_get_background(struct ui_box* box);

// -----------------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------------

void
ui_box_set_width(
  struct ui_box* box,
  int new_width,
  enum UI_SIZE_TYPE new_width_type
);

void
ui_box_set_height(
  struct ui_box* box,
  int new_height,
  enum UI_SIZE_TYPE new_height_type
);

void
ui_box_set_background(struct ui_box* box, int new_background);

#endif
