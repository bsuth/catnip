#ifndef CATNIP_OUTPUT_LAYOUT_H
#define CATNIP_OUTPUT_LAYOUT_H

#include "output.h"
#include <wlr/types/wlr_output_layout.h>

extern struct wlr_output_layout* catnip_output_layout;
extern struct wlr_scene_output_layout* catnip_scene_output_layout;

struct catnip_output*
catnip_output_layout_output_at(double x, double y);

struct catnip_output*
catnip_output_layout_get_focused_output();

void
catnip_output_layout_init();

#endif
