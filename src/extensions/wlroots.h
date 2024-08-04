#ifndef CATNIP_EXTENSIONS_WLROOTS_H
#define CATNIP_EXTENSIONS_WLROOTS_H

#include <wlr/types/wlr_scene.h>

int
wlr_scene_node_get_zindex(struct wlr_scene_node* node);

void
wlr_scene_node_set_zindex(struct wlr_scene_node* node, int zindex);

#endif
