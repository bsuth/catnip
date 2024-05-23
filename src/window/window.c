#include "window.h"
#include "config.h"
#include "id.h"
#include "scene.h"
#include "utils/wayland.h"
#include "window/lua_window.h"
#include <stdlib.h>

static void
on_window_map(struct wl_listener* listener, void* data)
{
  struct catnip_window* window =
    wl_container_of(listener, window, map_listener);

  // Do not create the Lua window until the surface has actually been mapped,
  // i.e. initialization has finished and the window is ready to be shown on
  // screen.
  lua_catnip_window_create(catnip_L, window);
}

static void
on_window_unmap(struct wl_listener* listener, void* data)
{
  struct catnip_window* window =
    wl_container_of(listener, window, unmap_listener);

  // Analogous to creating the Lua window once the window has been mapped, we
  // destroy the Lua window during unmap. After this, the window is no longer
  // visible on screen.
  lua_catnip_window_destroy(catnip_L, window->lua_resource);
}

static void
on_window_request_maximize(struct wl_listener* listener, void* data)
{
  struct catnip_window* window =
    wl_container_of(listener, window, request_maximize_listener);

  lua_catnip_resource_publish(catnip_L, window->lua_resource, "maximize", 0);

  // The xdg-shell protocol requires us to send a configure. We use
  // `wlr_xdg_surface_schedule_configure` to just send an empty reply.
  wlr_xdg_surface_schedule_configure(window->xdg_toplevel->base);
}

static void
on_window_request_fullscreen(struct wl_listener* listener, void* data)
{
  struct catnip_window* window =
    wl_container_of(listener, window, request_fullscreen_listener);

  lua_catnip_resource_publish(catnip_L, window->lua_resource, "fullscreen", 0);

  // The xdg-shell protocol requires us to send a configure. We use
  // `wlr_xdg_surface_schedule_configure` to just send an empty reply.
  wlr_xdg_surface_schedule_configure(window->xdg_toplevel->base);
}

static void
on_window_destroy(struct wl_listener* listener, void* data)
{
  struct catnip_window* window =
    wl_container_of(listener, window, destroy_listener);

  wl_list_remove(&window->link);
  wl_list_remove(&window->map_listener.link);
  wl_list_remove(&window->unmap_listener.link);
  wl_list_remove(&window->request_maximize_listener.link);
  wl_list_remove(&window->request_fullscreen_listener.link);
  wl_list_remove(&window->destroy_listener.link);

  free(window);
}

struct catnip_window*
catnip_window_create(struct wlr_xdg_surface* xdg_surface)
{
  struct catnip_window* window = calloc(1, sizeof(struct catnip_window));

  window->id = generate_catnip_id();
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
    &window->map_listener,
    &window->xdg_surface->surface->events.map,
    on_window_map
  );
  wl_setup_listener(
    &window->unmap_listener,
    &window->xdg_surface->surface->events.unmap,
    on_window_unmap
  );
  wl_setup_listener(
    &window->request_maximize_listener,
    &window->xdg_toplevel->events.request_maximize,
    on_window_request_maximize
  );
  wl_setup_listener(
    &window->request_fullscreen_listener,
    &window->xdg_toplevel->events.request_fullscreen,
    on_window_request_fullscreen
  );
  wl_setup_listener(
    &window->destroy_listener,
    &window->xdg_surface->events.destroy,
    on_window_destroy
  );

  return window;
}
