#include "cursor_methods.h"
#include "cursor/cursor.h"
#include "scene.h"
#include "seat/seat.h"

void
catnip_cursor_notify(uint32_t time_msec)
{
  double sx = 0;
  double sy = 0;

  struct wlr_scene_node* target_node = wlr_scene_node_at(
    &catnip_scene->tree.node,
    catnip_cursor->x,
    catnip_cursor->y,
    &sx,
    &sy
  );

  if (target_node == NULL || target_node->type != WLR_SCENE_NODE_BUFFER) {
    wlr_seat_pointer_clear_focus(catnip_seat);
  } else {
    struct wlr_scene_buffer* target_buffer =
      wlr_scene_buffer_from_node(target_node);

    struct wlr_scene_surface* target_scene_surface =
      wlr_scene_surface_try_from_buffer(target_buffer);

    if (target_scene_surface != NULL) {
      wlr_seat_pointer_notify_enter(
        catnip_seat,
        target_scene_surface->surface,
        sx,
        sy
      );
    }

    wlr_seat_pointer_notify_motion(catnip_seat, time_msec, sx, sy);
  }
}
