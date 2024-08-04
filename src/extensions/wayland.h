#ifndef CATNIP_EXTENSIONS_WAYLAND_H
#define CATNIP_EXTENSIONS_WAYLAND_H

#include <wayland-server-core.h>
#include <wlr/types/wlr_scene.h>

void
wl_signal_subscribe(
  struct wl_signal* signal,
  struct wl_listener* listener,
  wl_notify_func_t callback
);

int
wlr_scene_node_get_zindex(struct wlr_scene_node* node);

void
wlr_scene_node_set_zindex(struct wlr_scene_node* node, int zindex);

#endif
