#include "window.h"
#include "api/window.h"
#include "server/output_layout.h"
#include "server/scene.h"
#include "server/seat.h"
#include "utils/wayland.h"
#include <stdlib.h>
#include <wayland-util.h>

struct wl_list server_windows;
struct server_window_events server_window_events;

static struct {
  struct wl_listener xdg_shell_new_surface;
} listeners;

// -----------------------------------------------------------------------------
// Server Window
// -----------------------------------------------------------------------------

static void
on_xdg_surface_map(struct wl_listener* listener, void* data)
{
  struct server_window* window =
    wl_container_of(listener, window, listeners.map);
  server_window_set_focused(window, true);
}

static void
on_xdg_surface_unmap(struct wl_listener* listener, void* data)
{
  struct server_window* window =
    wl_container_of(listener, window, listeners.unmap);

  // TODO
  // Reset the cursor mode if the grabbed view was unmapped.
  // if (toplevel == toplevel->server->grabbed_view) {
  //   reset_cursor_mode(toplevel->server);
  // }
}

static void
on_xdg_surface_destroy(struct wl_listener* listener, void* data)
{
  struct server_window* window =
    wl_container_of(listener, window, listeners.destroy);

  wl_signal_emit_mutable(&window->events.destroy, window);

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
on_xdg_toplevel_request_move(struct wl_listener* listener, void* data)
{
  // TODO check the provided serial against a list of button press serials sent
  // to this client, to prevent the client from requesting this whenever they
  // want.
  struct server_window* window =
    wl_container_of(listener, window, listeners.request_move);
  // TODO start move
}

static void
on_xdg_toplevel_request_resize(struct wl_listener* listener, void* data)
{
  // TODO check the provided serial against a list of button press serials sent
  // to this client, to prevent the client from requesting this whenever they
  // want.
  struct wlr_xdg_toplevel_resize_event* event = data;
  struct server_window* window =
    wl_container_of(listener, window, listeners.request_resize);
  // TODO start resize
}

static void
on_xdg_toplevel_request_maximize(struct wl_listener* listener, void* data)
{
  // TODO support maximization
  struct server_window* window =
    wl_container_of(listener, window, listeners.request_maximize);
  wlr_xdg_surface_schedule_configure(window->xdg_toplevel->base);
}

static void
on_xdg_toplevel_request_fullscreen(struct wl_listener* listener, void* data)
{
  // TODO support fullscreen
  struct server_window* window =
    wl_container_of(listener, window, listeners.request_fullscreen);
  wlr_xdg_surface_schedule_configure(window->xdg_toplevel->base);
}

static void
server_window_create(struct wlr_xdg_surface* xdg_surface)
{
  if (xdg_surface->role == WLR_XDG_SURFACE_ROLE_POPUP) {
    struct wlr_xdg_surface* parent =
      wlr_xdg_surface_from_wlr_surface(xdg_surface->popup->parent);

    // wlroots provides a helper for adding xdg popups to the scene graph, but
    // it requires the popup parent's scene node. For convenience, we always
    // store the scene node in `xdg_surface->data`.
    xdg_surface->data = wlr_scene_xdg_surface_create(parent->data, xdg_surface);
  } else if (xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
    struct server_window* window = calloc(1, sizeof(struct server_window));

    window->xdg_surface = xdg_surface;
    window->xdg_toplevel = xdg_surface->toplevel;

    window->scene_tree = wlr_scene_xdg_surface_create(
      &server_scene->tree,
      window->xdg_toplevel->base
    );
    window->scene_tree->node.data = window;

    wl_signal_init(&window->events.destroy);

    wl_setup_listener(
      &window->listeners.map,
      &window->xdg_surface->events.map,
      on_xdg_surface_map
    );
    wl_setup_listener(
      &window->listeners.unmap,
      &window->xdg_surface->events.unmap,
      on_xdg_surface_unmap
    );
    wl_setup_listener(
      &window->listeners.destroy,
      &window->xdg_surface->events.destroy,
      on_xdg_surface_destroy
    );
    wl_setup_listener(
      &window->listeners.request_move,
      &window->xdg_toplevel->events.request_move,
      on_xdg_toplevel_request_move
    );
    wl_setup_listener(
      &window->listeners.request_resize,
      &window->xdg_toplevel->events.request_resize,
      on_xdg_toplevel_request_resize
    );
    wl_setup_listener(
      &window->listeners.request_maximize,
      &window->xdg_toplevel->events.request_maximize,
      on_xdg_toplevel_request_maximize
    );
    wl_setup_listener(
      &window->listeners.request_fullscreen,
      &window->xdg_toplevel->events.request_fullscreen,
      on_xdg_toplevel_request_fullscreen
    );

    // wlroots provides a helper for adding xdg popups to the scene graph, but
    // it requires the popup parent's scene node. For convenience, we always
    // store the scene node in `xdg_surface->data`.
    xdg_surface->data = window->scene_tree;

    wl_list_insert(&server_windows, &window->link);
    wl_signal_emit_mutable(&server_window_events.new_server_window, window);
  }
}

struct server_window*
server_window_get_at(double x, double y, double* nx, double* ny)
{
  struct wlr_scene_tree* root = &server_scene->tree;

  struct wlr_scene_node* node = wlr_scene_node_at(&root->node, x, y, nx, ny);

  if (node == NULL || node->type != WLR_SCENE_NODE_BUFFER) {
    return NULL;
  }

  struct wlr_scene_tree* tree = node->parent;

  while (tree != NULL && tree->node.data == NULL) {
    tree = tree->node.parent;
  }

  return tree == NULL ? NULL : tree->node.data;
}

// -----------------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------------

int
server_window_get_lx(struct server_window* window)
{
  int lx, ly;
  wlr_scene_node_coords(&window->scene_tree->node, &lx, &ly);
  return lx;
}

int
server_window_get_ly(struct server_window* window)
{
  int lx, ly;
  wlr_scene_node_coords(&window->scene_tree->node, &lx, &ly);
  return ly;
}

int
server_window_get_gx(struct server_window* window)
{
  struct wlr_surface_output* surface_output;
  struct wl_list* surface_outputs =
    &window->xdg_surface->surface->current_outputs;

  wl_list_for_each(surface_output, surface_outputs, link)
  {
    struct wlr_output_layout_output* layout_output =
      wlr_output_layout_get(server_output_layout, surface_output->output);

    if (layout_output != NULL) {
      return layout_output->x + server_window_get_lx(window);
    }
  }

  return 0;
}

int
server_window_get_gy(struct server_window* window)
{
  struct wlr_surface_output* surface_output;
  struct wl_list* surface_outputs =
    &window->xdg_surface->surface->current_outputs;

  wl_list_for_each(surface_output, surface_outputs, link)
  {
    struct wlr_output_layout_output* layout_output =
      wlr_output_layout_get(server_output_layout, surface_output->output);

    if (layout_output != NULL) {
      return layout_output->y + server_window_get_ly(window);
    }
  }

  return 0;
}

int
server_window_get_width(struct server_window* window)
{
  struct wlr_box box;
  wlr_xdg_surface_get_geometry(window->xdg_toplevel->base, &box);
  return box.width;
}

int
server_window_get_height(struct server_window* window)
{
  struct wlr_box box;
  wlr_xdg_surface_get_geometry(window->xdg_toplevel->base, &box);
  return box.height;
}

bool
server_window_get_focused(struct server_window* window)
{
  return window->xdg_toplevel->base->surface
         == server_seat->keyboard_state.focused_surface;
}

bool
server_window_get_maximized(struct server_window* window)
{
  // TODO
  return false;
}

bool
server_window_get_fullscreen(struct server_window* window)
{
  // TODO
  return false;
}

// -----------------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------------

void
server_window_set_lx(struct server_window* window, int new_lx)
{
  wlr_scene_node_set_position(
    &window->scene_tree->node,
    new_lx,
    server_window_get_ly(window)
  );
}

void
server_window_set_ly(struct server_window* window, int new_ly)
{
  wlr_scene_node_set_position(
    &window->scene_tree->node,
    server_window_get_lx(window),
    new_ly
  );
}

void
server_window_set_gx(struct server_window* window, int new_gx)
{
  // TODO
}

void
server_window_set_gy(struct server_window* window, int new_gy)
{
  // TODO
}

void
server_window_set_width(struct server_window* window, int new_width)
{
  wlr_xdg_toplevel_set_size(
    window->xdg_toplevel,
    new_width,
    server_window_get_height(window)
  );
}

void
server_window_set_height(struct server_window* window, int new_height)
{
  wlr_xdg_toplevel_set_size(
    window->xdg_toplevel,
    server_window_get_width(window),
    new_height
  );
}

void
server_window_set_focused(struct server_window* window, bool new_focused)
{
  if (server_window_get_focused(window) == new_focused) {
    return; // nothing to do
  } else if (new_focused == false) {
    wlr_xdg_toplevel_set_activated(window->xdg_toplevel, false);
    wlr_seat_keyboard_notify_clear_focus(server_seat);
  } else {
    wlr_scene_node_raise_to_top(&window->scene_tree->node);
    wlr_xdg_toplevel_set_activated(window->xdg_toplevel, true);

    struct wlr_keyboard* keyboard = wlr_seat_get_keyboard(server_seat);

    if (keyboard != NULL) {
      wlr_seat_keyboard_notify_enter(
        server_seat,
        window->xdg_toplevel->base->surface,
        keyboard->keycodes,
        keyboard->num_keycodes,
        &keyboard->modifiers
      );
    }
  }
}

void
server_window_set_maximized(struct server_window* window, bool new_maximized)
{
  // TODO
}

void
server_window_set_fullscreen(struct server_window* window, bool new_fullscreen)
{
  // TODO
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

static void
on_xdg_shell_new_surface(struct wl_listener* listener, void* data)
{
  server_window_create(data);
}

void
server_window_init()
{
  wl_list_init(&server_windows);

  wl_signal_init(&server_window_events.new_server_window);

  wl_setup_listener(
    &listeners.xdg_shell_new_surface,
    &server_xdg_shell->events.new_surface,
    on_xdg_shell_new_surface
  );
}
