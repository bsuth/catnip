#include "scene.h"
#include "server/output_layout.h"

struct wlr_scene* server_scene;

void
init_server_scene()
{
  server_scene = wlr_scene_create();
  wlr_scene_attach_output_layout(server_scene, server_output_layout);
}
