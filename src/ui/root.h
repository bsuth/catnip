#ifndef CATNIP_UI_ROOT_H
#define CATNIP_UI_ROOT_H

#include "ui/properties.h"
#include "ui/types.h"
#include <cairo.h>
#include <stdbool.h>
#include <wlr/types/wlr_scene.h>

struct ui_root {
  // WLR Scene State
  struct wlr_buffer buffer;
  struct wlr_scene_tree* scene_tree;
  struct wlr_scene_buffer* scene_buffer;

  // Cairo State
  cairo_surface_t* surface;
  cairo_t* cr;

  // Properties
  struct ui_dimension width;
  struct ui_dimension height;
  bool visible;

  // Computed Properties
  int computed_width;
  int computed_height;
};

struct ui_root*
create_ui_root();

void
ui_root_refresh(struct ui_root* root);

#endif
