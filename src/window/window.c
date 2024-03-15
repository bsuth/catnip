#include "window.h"
#include "config.h"
#include "scene.h"
#include "seat/seat.h"
#include "utils/wayland.h"
#include "window/lua_window.h"
#include "xdg_shell.h"
#include <stdlib.h>

struct wl_list catnip_windows;

static struct {
  struct wl_listener new_xdg_surface;
  struct wl_listener keyboard_focus_change;
} listeners;

static void
catnip_window_map(struct wl_listener* listener, void* data)
{
  struct catnip_window* window =
    wl_container_of(listener, window, listeners.map);

  // Initialize `xdg_toplevel->scheduled` to match the `xdg_toplevel` initial
  // state since we always query `xdg_toplevel->scheduled` for window
  // properties.
  window->xdg_toplevel->scheduled.width =
    window->xdg_surface->surface->current.width;
  window->xdg_toplevel->scheduled.height =
    window->xdg_surface->surface->current.height;

  lua_catnip_window_create(catnip_L, window);

  // Do not add the window to the `catnip_windows` list until it has actually
  // been mapped. This is to prevent double creating the Lua window in the case
  // that the user refreshes the Lua state after the window has been created
  // (i.e. `catnip_window_create`) but before the window has been mapped.
  wl_list_insert(&catnip_windows, &window->link);
}

static void
catnip_window_unmap(struct wl_listener* listener, void* data)
{
  struct catnip_window* window =
    wl_container_of(listener, window, listeners.unmap);

  lua_catnip_window_destroy(catnip_L, window->lua_resource);

  wl_list_remove(&window->link);
}

static void
catnip_window_configure(struct wl_listener* listener, void* data)
{
  struct catnip_window* window =
    wl_container_of(listener, window, listeners.configure);

  struct wlr_xdg_surface_configure* configure = data;
  struct wlr_xdg_toplevel_configure* toplevel_configure =
    configure->toplevel_configure;

  if (toplevel_configure->activated != window->latest.focused) {
    window->latest.focused = toplevel_configure->activated;
    lua_catnip_resource_publish(
      catnip_L,
      window->lua_resource,
      "property::focused",
      0
    );
  }

  // Sync the `activated` state with whether this windows surface is the focused
  // keyboard surface. Note that this should be done when handling the
  // `configure` event and _not_ when updating from Lua, since
  // `wlr_seat_keyboard_notify_enter` will queue events immediately and queueing
  // multiple `focus_change` events with varying surfaces will cause the earlier
  // events to incorrectly sync the `activated` properties.
  if (toplevel_configure->activated != window->latest.activated) {
    window->latest.activated = toplevel_configure->activated;

    struct wlr_surface* window_surface = window->xdg_toplevel->base->surface;
    struct wlr_surface* focused_surface =
      catnip_seat->keyboard_state.focused_surface;

    if (toplevel_configure->activated != (window_surface == focused_surface)) {
      if (!toplevel_configure->activated) {
        wlr_seat_keyboard_notify_clear_focus(catnip_seat);
      } else {
        struct wlr_keyboard* keyboard = wlr_seat_get_keyboard(catnip_seat);
        if (keyboard != NULL) {
          wlr_seat_keyboard_notify_enter(
            catnip_seat,
            window_surface,
            keyboard->keycodes,
            keyboard->num_keycodes,
            &keyboard->modifiers
          );
        }
      }
    }
  }
}

static void
catnip_window_request_move(struct wl_listener* listener, void* data)
{
  struct wlr_xdg_toplevel_move_event* event = data;

  struct catnip_window* window =
    wl_container_of(listener, window, listeners.request_move);

  // TODO
}

static void
catnip_window_request_resize(struct wl_listener* listener, void* data)
{
  struct wlr_xdg_toplevel_resize_event* event = data;

  struct catnip_window* window =
    wl_container_of(listener, window, listeners.request_resize);

  // TODO
}

static void
catnip_window_request_maximize(struct wl_listener* listener, void* data)
{
  struct catnip_window* window =
    wl_container_of(listener, window, listeners.request_maximize);

  lua_catnip_resource_publish(catnip_L, window->lua_resource, "maximize", 0);

  // The xdg-shell protocol requires us to send a configure. We use
  // `wlr_xdg_surface_schedule_configure` to just send an empty reply.
  wlr_xdg_surface_schedule_configure(window->xdg_toplevel->base);
}

static void
catnip_window_request_fullscreen(struct wl_listener* listener, void* data)
{
  struct catnip_window* window =
    wl_container_of(listener, window, listeners.request_fullscreen);

  lua_catnip_resource_publish(catnip_L, window->lua_resource, "fullscreen", 0);

  // The xdg-shell protocol requires us to send a configure. We use
  // `wlr_xdg_surface_schedule_configure` to just send an empty reply.
  wlr_xdg_surface_schedule_configure(window->xdg_toplevel->base);
}

static void
catnip_window_destroy(struct wl_listener* listener, void* data)
{
  struct catnip_window* window =
    wl_container_of(listener, window, listeners.destroy);

  wl_list_remove(&window->listeners.map.link);
  wl_list_remove(&window->listeners.unmap.link);
  wl_list_remove(&window->listeners.destroy.link);
  wl_list_remove(&window->listeners.request_move.link);
  wl_list_remove(&window->listeners.request_resize.link);
  wl_list_remove(&window->listeners.request_maximize.link);
  wl_list_remove(&window->listeners.request_fullscreen.link);

  free(window);
}

static void
catnip_window_create(struct wl_listener* listener, void* data)
{
  struct wlr_xdg_surface* xdg_surface = data;

  if (xdg_surface->role == WLR_XDG_SURFACE_ROLE_POPUP) {
    struct wlr_xdg_surface* parent =
      wlr_xdg_surface_try_from_wlr_surface(xdg_surface->popup->parent);

    // wlroots provides a helper for adding xdg popups to the scene graph, but
    // it requires the popup parent's scene node. For convenience, we always
    // store the scene node in `xdg_surface->data`.
    xdg_surface->data = wlr_scene_xdg_surface_create(parent->data, xdg_surface);
  } else if (xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
    struct catnip_window* window = calloc(1, sizeof(struct catnip_window));

    window->xdg_surface = xdg_surface;
    window->xdg_toplevel = xdg_surface->toplevel;
    window->scene_tree = wlr_scene_xdg_surface_create(
      &catnip_scene->tree,
      window->xdg_toplevel->base
    );

    // wlroots provides a helper for adding xdg popups to the scene graph, but
    // it requires the popup parent's scene node. For convenience, we always
    // store the scene node in `xdg_surface->data`.
    xdg_surface->data = window->scene_tree;

    wl_setup_listener(
      &window->listeners.map,
      &window->xdg_surface->surface->events.map,
      catnip_window_map
    );
    wl_setup_listener(
      &window->listeners.configure,
      &window->xdg_surface->events.configure,
      catnip_window_configure
    );
    wl_setup_listener(
      &window->listeners.request_move,
      &window->xdg_toplevel->events.request_move,
      catnip_window_request_move
    );
    wl_setup_listener(
      &window->listeners.request_resize,
      &window->xdg_toplevel->events.request_resize,
      catnip_window_request_resize
    );
    wl_setup_listener(
      &window->listeners.request_maximize,
      &window->xdg_toplevel->events.request_maximize,
      catnip_window_request_maximize
    );
    wl_setup_listener(
      &window->listeners.request_fullscreen,
      &window->xdg_toplevel->events.request_fullscreen,
      catnip_window_request_fullscreen
    );
    wl_setup_listener(
      &window->listeners.destroy,
      &window->xdg_surface->events.destroy,
      catnip_window_destroy
    );
  }
}

static void
catnip_window_sync_keyboard_focus(struct wl_listener* listener, void* data)
{
  struct wlr_seat_keyboard_focus_change_event* event = data;

  if (event->old_surface != NULL) {
    struct wlr_xdg_toplevel* old_toplevel =
      wlr_xdg_toplevel_try_from_wlr_surface(event->old_surface);

    if (old_toplevel != NULL && old_toplevel->base->initialized && old_toplevel->scheduled.activated) {
      wlr_xdg_toplevel_set_activated(old_toplevel, false);
    }
  }

  if (event->new_surface != NULL) {
    struct wlr_xdg_toplevel* new_toplevel =
      wlr_xdg_toplevel_try_from_wlr_surface(event->new_surface);

    if (new_toplevel != NULL && new_toplevel->base->initialized && new_toplevel->scheduled.activated) {
      wlr_xdg_toplevel_set_activated(new_toplevel, true);
    }
  }
}

void
catnip_window_init()
{
  wl_list_init(&catnip_windows);

  wl_setup_listener(
    &listeners.new_xdg_surface,
    &catnip_xdg_shell->events.new_surface,
    catnip_window_create
  );

  wl_setup_listener(
    &listeners.keyboard_focus_change,
    &catnip_seat->keyboard_state.events.focus_change,
    catnip_window_sync_keyboard_focus
  );
}
