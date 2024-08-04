#include "init.h"
#include "seat.h"
#include "utils/wayland.h"
#include "window/window.h"
#include "window/windows.h"
#include "xdg_shell.h"

static struct wl_listener new_xdg_toplevel_listener;
static struct wl_listener keyboard_focus_change_listener;

static void
on_new_xdg_toplevel_listener(struct wl_listener* listener, void* data)
{
  struct wlr_xdg_toplevel* xdg_toplevel = data;
  struct catnip_window* window = catnip_window_create(xdg_toplevel);
  wl_list_insert(&catnip_windows, &window->link);
}

static void
on_keyboard_focus_change(struct wl_listener* listener, void* data)
{
  struct wlr_seat_keyboard_focus_change_event* event = data;

  if (event->old_surface != NULL) {
    struct wlr_xdg_toplevel* old_toplevel =
      wlr_xdg_toplevel_try_from_wlr_surface(event->old_surface);

    if (old_toplevel != NULL && old_toplevel->base->initialized
        && old_toplevel->scheduled.activated) {
      wlr_xdg_toplevel_set_activated(old_toplevel, false);
    }
  }

  if (event->new_surface != NULL) {
    struct wlr_xdg_toplevel* new_toplevel =
      wlr_xdg_toplevel_try_from_wlr_surface(event->new_surface);

    if (new_toplevel != NULL && new_toplevel->base->initialized
        && new_toplevel->scheduled.activated) {
      wlr_xdg_toplevel_set_activated(new_toplevel, true);
    }
  }
}

void
catnip_window_init()
{
  catnip_windows_init();

  // TODO: handle popups

  wl_signal_subscribe(
    &catnip_xdg_shell->events.new_toplevel,
    &new_xdg_toplevel_listener,
    on_new_xdg_toplevel_listener
  );

  wl_signal_subscribe(
    &catnip_seat->keyboard_state.events.focus_change,
    &keyboard_focus_change_listener,
    on_keyboard_focus_change
  );
}
