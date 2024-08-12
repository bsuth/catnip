#ifndef CATNIP_DESKTOP_OUTPUTS_H
#define CATNIP_DESKTOP_OUTPUTS_H

#include <wayland-util.h>
#include <wlr/types/wlr_output_layout.h>

extern struct wl_list catnip_outputs;
extern struct wlr_output_layout* catnip_output_layout;
extern struct wlr_scene_output_layout* catnip_scene_output_layout;

void
catnip_outputs_init();

#endif
