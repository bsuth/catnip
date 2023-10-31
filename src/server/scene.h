#ifndef CATNIP_SERVER_SCENE_H
#define CATNIP_SERVER_SCENE_H

#include <wlr/types/wlr_scene.h>

extern struct wlr_scene* server_scene;

void
server_scene_init();

#endif
