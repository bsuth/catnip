#include "init.h"
#include "seat.h"
#include "utils/wayland.h"
#include "window/window.h"
#include "window/windows.h"
#include "xdg_shell.h"

static struct wl_listener new_xdg_surface_listener;
static struct wl_listener keyboard_focus_change_listener;

static void
on_new_xdg_surface_listener(struct wl_listener* listener, void* data)
{
  struct wlr_xdg_surface* xdg_surface = data;

  // TODO: handle popups
  if (xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
    struct catnip_window* window = catnip_window_create(xdg_surface);
    wl_list_insert(&catnip_windows, &window->link);
  }
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

  wl_setup_listener(
    &new_xdg_surface_listener,
    &catnip_xdg_shell->events.new_surface,
    on_new_xdg_surface_listener
  );

  wl_setup_listener(
    &keyboard_focus_change_listener,
    &catnip_seat->keyboard_state.events.focus_change,
    on_keyboard_focus_change
  );
}
