#include "cursor.h"
#include "output/output.h"
#include "output/outputs.h"
#include "scene.h"
#include "seat.h"
#include "utils/time.h"

struct wlr_cursor* catnip_cursor = NULL;
struct wlr_xcursor_manager* catnip_xcursor_manager = NULL;
char* catnip_cursor_name = NULL;

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

void
catnip_cursor_xcursor(const char* name, int size)
{
  struct wlr_xcursor_manager* new_wlr_xcursor_manager =
    wlr_xcursor_manager_create(name, size);

  // Always load scale 1
  wlr_xcursor_manager_load(new_wlr_xcursor_manager, 1);

  // Ensure we have loaded a scaled theme for each output's scale
  struct catnip_output* output = NULL;
  wl_list_for_each(output, &catnip_outputs, link)
  {
    wlr_xcursor_manager_load(
      new_wlr_xcursor_manager,
      output->wlr_output->scale
    );
  }

  wlr_xcursor_manager_destroy(catnip_xcursor_manager);
  catnip_xcursor_manager = new_wlr_xcursor_manager;
}

void
catnip_cursor_move(double x, double y)
{
  wlr_cursor_warp_closest(catnip_cursor, NULL, x, y);
  catnip_cursor_notify(get_time_msec());
}
