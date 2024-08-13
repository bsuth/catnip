#include "window.h"
#include "compositor/scene.h"
#include "compositor/seat.h"
#include "config.h"
#include "desktop/lua_window.h"
#include "extensions/wayland.h"
#include "id.h"
#include <stdlib.h>

// -----------------------------------------------------------------------------
// Create
// -----------------------------------------------------------------------------

static void
on_window_map(struct wl_listener* listener, void* data)
{
  struct catnip_window* window =
    wl_container_of(listener, window, listeners.surface_map);

  // Do not create the Lua window until the surface has actually been mapped,
  // i.e. initialization has finished and the window is ready to be shown on
  // screen.
  catnip_lua_window_create(catnip_L, window);
}

static void
on_window_commit(struct wl_listener* listener, void* data)
{
  struct catnip_window* window =
    wl_container_of(listener, window, listeners.surface_commit);

  if (window->wlr.xdg_toplevel->base->initial_commit) {
    // When an xdg_surface performs an initial commit, the compositor must
    // reply with a configure so the client can map the surface.
    wlr_xdg_surface_schedule_configure(window->wlr.xdg_toplevel->base);
  }
}

static void
on_window_unmap(struct wl_listener* listener, void* data)
{
  struct catnip_window* window =
    wl_container_of(listener, window, listeners.surface_unmap);

  // Analogous to creating the Lua window once the window has been mapped, we
  // destroy the Lua window during unmap. After this, the window is no longer
  // visible on screen.
  catnip_lua_window_destroy(catnip_L, window->lua_resource);
}

static void
on_window_request_maximize(struct wl_listener* listener, void* data)
{
  struct catnip_window* window =
    wl_container_of(listener, window, listeners.xdg_toplevel_request_maximize);

  catnip_lua_resource_publish(catnip_L, window->lua_resource, "maximize", 0);

  // The xdg-shell protocol requires us to send a configure. We use
  // `wlr_xdg_surface_schedule_configure` to just send an empty reply.
  wlr_xdg_surface_schedule_configure(window->wlr.xdg_toplevel->base);
}

static void
on_window_request_fullscreen(struct wl_listener* listener, void* data)
{
  struct catnip_window* window = wl_container_of(
    listener,
    window,
    listeners.xdg_toplevel_request_fullscreen
  );

  catnip_lua_resource_publish(catnip_L, window->lua_resource, "fullscreen", 0);

  // The xdg-shell protocol requires us to send a configure. We use
  // `wlr_xdg_surface_schedule_configure` to just send an empty reply.
  wlr_xdg_surface_schedule_configure(window->wlr.xdg_toplevel->base);
}

static void
on_window_destroy(struct wl_listener* listener, void* data)
{
  struct catnip_window* window =
    wl_container_of(listener, window, listeners.xdg_toplevel_destroy);

  wl_list_remove(&window->link);
  wl_list_remove(&window->listeners.surface_map.link);
  wl_list_remove(&window->listeners.surface_commit.link);
  wl_list_remove(&window->listeners.surface_unmap.link);
  wl_list_remove(&window->listeners.xdg_toplevel_request_maximize.link);
  wl_list_remove(&window->listeners.xdg_toplevel_request_fullscreen.link);
  wl_list_remove(&window->listeners.xdg_toplevel_destroy.link);

  free(window);
}

struct catnip_window*
catnip_window_create(struct wlr_xdg_toplevel* xdg_toplevel)
{
  struct catnip_window* window = calloc(1, sizeof(struct catnip_window));
  struct wlr_xdg_surface* xdg_surface = xdg_toplevel->base;

  window->id = generate_catnip_id();
  window->wlr.xdg_toplevel = xdg_toplevel;
  window->wlr.scene_tree =
    wlr_scene_xdg_surface_create(&catnip_scene->tree, xdg_surface);

  // wlroots provides a helper for adding xdg popups to the scene graph, but
  // it requires the popup parent's scene node. For convenience, we always
  // store the scene node in `xdg_surface->data`.
  xdg_surface->data = window->wlr.scene_tree;

  wl_signal_subscribe(
    &xdg_surface->surface->events.map,
    &window->listeners.surface_map,
    on_window_map
  );
  wl_signal_subscribe(
    &xdg_surface->surface->events.commit,
    &window->listeners.surface_commit,
    on_window_commit
  );
  wl_signal_subscribe(
    &xdg_surface->surface->events.unmap,
    &window->listeners.surface_unmap,
    on_window_unmap
  );
  wl_signal_subscribe(
    &xdg_toplevel->events.request_maximize,
    &window->listeners.xdg_toplevel_request_maximize,
    on_window_request_maximize
  );
  wl_signal_subscribe(
    &xdg_toplevel->events.request_fullscreen,
    &window->listeners.xdg_toplevel_request_fullscreen,
    on_window_request_fullscreen
  );
  wl_signal_subscribe(
    &xdg_toplevel->events.destroy,
    &window->listeners.xdg_toplevel_destroy,
    on_window_destroy
  );

  return window;
}

// -----------------------------------------------------------------------------
// Methods
// -----------------------------------------------------------------------------

void
catnip_window_focus(struct catnip_window* window)
{
  struct wlr_keyboard* keyboard = wlr_seat_get_keyboard(catnip_seat);

  if (keyboard != NULL) {
    wlr_seat_keyboard_notify_enter(
      catnip_seat,
      window->wlr.xdg_toplevel->base->surface,
      keyboard->keycodes,
      keyboard->num_keycodes,
      &keyboard->modifiers
    );
  }
}
