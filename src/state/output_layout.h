#ifndef CATNIP_STATE_OUTPUT_LAYOUT_H
#define CATNIP_STATE_OUTPUT_LAYOUT_H

#include <wlr/types/wlr_output_layout.h>

extern struct wlr_output_layout* catnip_output_layout;
extern struct wlr_scene_output_layout* catnip_scene_output_layout;

void
catnip_output_layout_init();

#endif
