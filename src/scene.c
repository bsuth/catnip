#include "scene.h"

struct wlr_scene* catnip_scene = NULL;

void
catnip_scene_init()
{
  catnip_scene = wlr_scene_create();
}
