#include "scene.h"
#include "output.h"

struct wlr_scene* server_scene;

void
server_scene_init()
{
  server_scene = wlr_scene_create();
  wlr_scene_attach_output_layout(server_scene, server_output_layout);
}
