#include "windows.h"
#include "extensions/wayland.h"
#include "state/seat.h"
#include "state/xdg_shell.h"
#include "window/window.h"

struct wl_list catnip_windows;

static struct {
  struct wl_listener xdg_shell_new_toplevel;
  struct wl_listener seat_keyboard_focus_change;
} listeners;

static void
on_xdg_shell_new_toplevel(struct wl_listener* listener, void* data)
{
  struct wlr_xdg_toplevel* xdg_toplevel = data;
  struct catnip_window* window = catnip_window_create(xdg_toplevel);
  wl_list_insert(&catnip_windows, &window->link);
}

static void
on_seat_keyboard_focus_change(struct wl_listener* listener, void* data)
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
catnip_windows_init()
{
  wl_list_init(&catnip_windows);

  wl_signal_subscribe(
    &catnip_xdg_shell->events.new_toplevel,
    &listeners.xdg_shell_new_toplevel,
    on_xdg_shell_new_toplevel
  );

  wl_signal_subscribe(
    &catnip_seat->keyboard_state.events.focus_change,
    &listeners.seat_keyboard_focus_change,
    on_seat_keyboard_focus_change
  );
}
