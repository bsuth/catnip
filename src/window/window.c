#include "window.h"
#include "config/config.h"
#include "scene.h"
#include "utils/wayland.h"
#include "window/lua_window.h"
#include "window/window_properties.h"
#include "xdg_shell.h"
#include <stdlib.h>

struct wl_list catnip_windows;

static struct {
  struct wl_listener new_xdg_surface;
} listeners;

static void
catnip_window_map(struct wl_listener* listener, void* data)
{
  struct catnip_window* window =
    wl_container_of(listener, window, listeners.map);
  catnip_window_set_focused(window, true);
}

static void
catnip_window_unmap(struct wl_listener* listener, void* data)
{
  struct catnip_window* window =
    wl_container_of(listener, window, listeners.unmap);

  // TODO
  // Reset the cursor mode if the grabbed view was unmapped.
  // if (toplevel == toplevel->server->grabbed_view) {
  //   reset_cursor_mode(toplevel->server);
  // }
}

static void
catnip_window_request_move(struct wl_listener* listener, void* data)
{
  // TODO check the provided serial against a list of button press serials sent
  // to this client, to prevent the client from requesting this whenever they
  // want.
  struct catnip_window* window =
    wl_container_of(listener, window, listeners.request_move);
  // TODO start move
}

static void
catnip_window_request_resize(struct wl_listener* listener, void* data)
{
  // TODO check the provided serial against a list of button press serials sent
  // to this client, to prevent the client from requesting this whenever they
  // want.
  struct wlr_xdg_toplevel_resize_event* event = data;
  struct catnip_window* window =
    wl_container_of(listener, window, listeners.request_resize);
  // TODO start resize
}

static void
catnip_window_request_maximize(struct wl_listener* listener, void* data)
{
  // TODO support maximization
  struct catnip_window* window =
    wl_container_of(listener, window, listeners.request_maximize);
  wlr_xdg_surface_schedule_configure(window->xdg_toplevel->base);
}

static void
catnip_window_request_fullscreen(struct wl_listener* listener, void* data)
{
  // TODO support fullscreen
  struct catnip_window* window =
    wl_container_of(listener, window, listeners.request_fullscreen);
  wlr_xdg_surface_schedule_configure(window->xdg_toplevel->base);
}

static void
catnip_window_destroy(struct wl_listener* listener, void* data)
{
  struct catnip_window* window =
    wl_container_of(listener, window, listeners.destroy);

  if (catnip_L != NULL) {
    lua_catnip_window_destroy(catnip_L, window);
  }

  wl_list_remove(&window->link);
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
    window->scene_tree->node.data = window;

    window->pending.width = -1;
    window->pending.height = -1;

    wl_setup_listener(
      &window->listeners.map,
      &window->xdg_surface->surface->events.map,
      catnip_window_map
    );
    wl_setup_listener(
      &window->listeners.unmap,
      &window->xdg_surface->surface->events.unmap,
      catnip_window_unmap
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

    // wlroots provides a helper for adding xdg popups to the scene graph, but
    // it requires the popup parent's scene node. For convenience, we always
    // store the scene node in `xdg_surface->data`.
    xdg_surface->data = window->scene_tree;

    wl_list_insert(&catnip_windows, &window->link);

    if (catnip_L != NULL) {
      lua_catnip_window_create(catnip_L, window);
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
}
