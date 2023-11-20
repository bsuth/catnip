#include "scene.h"
#include "output/output_layout.h"

struct wlr_scene* catnip_scene;

void
catnip_scene_init()
{
  catnip_scene = wlr_scene_create();
  wlr_scene_attach_output_layout(catnip_scene, catnip_output_layout);
}
