#include "wayland.h"

void
wl_setup_listener(
  struct wl_listener* listener,
  struct wl_signal* signal,
  wl_notify_func_t callback
)
{
  listener->notify = callback;
  wl_signal_add(signal, listener);
}

int
wlr_scene_node_get_zindex(struct wlr_scene_node* node)
{
  int child_zindex = 0;
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
  int child_zindex = 0;
  struct wlr_scene_node* child = NULL;

  if (zindex < 0) {
    wl_list_for_each_reverse(child, &node->parent->children, link)
    {
      if (--child_zindex == zindex) {
        wlr_scene_node_place_above(node, child);
        return;
      }
    }

    wlr_scene_node_lower_to_bottom(node);
  } else {
    wl_list_for_each(child, &node->parent->children, link)
    {
      if (++child_zindex == zindex) {
        wlr_scene_node_place_above(node, child);
        return;
      }
    }

    wlr_scene_node_raise_to_top(node);
  }
}
