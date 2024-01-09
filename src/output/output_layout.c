#include "output_layout.h"
#include "scene.h"

struct wlr_output_layout* catnip_output_layout = NULL;
struct wlr_scene_output_layout* catnip_scene_output_layout = NULL;

void
catnip_output_layout_init()
{
  catnip_output_layout = wlr_output_layout_create();
  catnip_scene_output_layout =
    wlr_scene_attach_output_layout(catnip_scene, catnip_output_layout);
}
