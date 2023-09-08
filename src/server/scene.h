#ifndef SERVER_SCENE_H
#define SERVER_SCENE_H

#include <wlr/types/wlr_scene.h>

extern struct wlr_scene* server_scene;

void
init_server_scene();

#endif
