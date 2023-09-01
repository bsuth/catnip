#include "scene.h"
#include "output.h"

struct wlr_scene* bwc_scene;

void
bwc_scene_init()
{
  bwc_scene = wlr_scene_create();
  wlr_scene_attach_output_layout(bwc_scene, bwc_output_layout);
}
