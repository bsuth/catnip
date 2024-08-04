#include "wlroots.h"

int
wlr_scene_node_get_zindex(struct wlr_scene_node* node)
{
  int child_zindex = 1;
  struct wlr_scene_node* child = NULL;

  wl_list_for_each(child, &node->parent->children, link)
  {
    if (child == node) {
      return child_zindex;
    }

    ++child_zindex;
  }

  return -1;
}

void
wlr_scene_node_set_zindex(struct wlr_scene_node* node, int zindex)
{
  int child_zindex = 1;
  struct wlr_scene_node* child = NULL;

  wl_list_for_each(child, &node->parent->children, link)
  {
    if (zindex < ++child_zindex) {
      if (node != child) {
        wlr_scene_node_place_below(node, child);
      }
      return;
    }
  }

  wlr_scene_node_raise_to_top(node);
}
