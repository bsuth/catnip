#ifndef CATNIP_COMPOSITOR_SCENE_H
#define CATNIP_COMPOSITOR_SCENE_H

#include <wlr/types/wlr_scene.h>

extern struct wlr_scene* catnip_scene;
extern struct wlr_scene_tree* catnip_scene_wallpaper_layer;
extern struct wlr_scene_tree* catnip_scene_desktop_layer;

void
catnip_scene_init();

#endif
