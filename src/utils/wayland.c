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
