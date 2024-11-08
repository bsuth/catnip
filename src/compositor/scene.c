#include "scene.h"

struct wlr_scene* catnip_scene = NULL;
struct wlr_scene_tree* catnip_scene_wallpaper_layer = NULL;

void
catnip_scene_init()
{
  catnip_scene = wlr_scene_create();
  catnip_scene_wallpaper_layer = wlr_scene_tree_create(&catnip_scene->tree);
}
