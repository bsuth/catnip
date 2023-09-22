#include "windows.h"
#include "lua_api/windows.h"
#include "server/output.h"
#include "server/scene.h"
#include "server/seat.h"
#include "utils/wayland.h"
#include <wayland-util.h>

static struct wl_listener server_xdg_shell_new_surface_listener;

// -----------------------------------------------------------------------------
// create_desktop_window
// -----------------------------------------------------------------------------

static void
desktop_window_xdg_surface_map(struct wl_listener* listener, void* data)
{
  struct desktop_window* window =
    wl_container_of(listener, window, map_listener);
  desktop_window_set_focused(window, true);
}

static void
desktop_window_xdg_surface_unmap(struct wl_listener* listener, void* data)
{
  struct desktop_window* window =
    wl_container_of(listener, window, unmap_listener);

  // TODO
  // Reset the cursor mode if the grabbed view was unmapped.
  // if (toplevel == toplevel->server->grabbed_view) {
  //   reset_cursor_mode(toplevel->server);
  // }
}

static void
desktop_window_xdg_surface_destroy(struct wl_listener* listener, void* data)
{
  struct desktop_window* window =
    wl_container_of(listener, window, destroy_listener);

  wl_list_remove(&window->map_listener.link);
  wl_list_remove(&window->unmap_listener.link);
  wl_list_remove(&window->destroy_listener.link);
  wl_list_remove(&window->request_move_listener.link);
  wl_list_remove(&window->request_resize_listener.link);
  wl_list_remove(&window->request_maximize_listener.link);
  wl_list_remove(&window->request_fullscreen_listener.link);

  free(window);
}

static void
desktop_window_xdg_toplevel_request_move(
  struct wl_listener* listener,
  void* data
)
{
  // TODO check the provided serial against a list of button press serials sent
  // to this client, to prevent the client from requesting this whenever they
  // want.
  struct desktop_window* window =
    wl_container_of(listener, window, request_move_listener);
  // TODO start move
}

static void
desktop_window_xdg_toplevel_request_resize(
  struct wl_listener* listener,
  void* data
)
{
  // TODO check the provided serial against a list of button press serials sent
  // to this client, to prevent the client from requesting this whenever they
  // want.
  struct wlr_xdg_toplevel_resize_event* event = data;
  struct desktop_window* window =
    wl_container_of(listener, window, request_resize_listener);
  // TODO start resize
}

static void
desktop_window_xdg_toplevel_request_maximize(
  struct wl_listener* listener,
  void* data
)
{
  // TODO support maximization
  struct desktop_window* window =
    wl_container_of(listener, window, request_maximize_listener);
  wlr_xdg_surface_schedule_configure(window->xdg_toplevel->base);
}

static void
desktop_window_xdg_toplevel_request_fullscreen(
  struct wl_listener* listener,
  void* data
)
{
  // TODO support fullscreen
  struct desktop_window* window =
    wl_container_of(listener, window, request_fullscreen_listener);
  wlr_xdg_surface_schedule_configure(window->xdg_toplevel->base);
}

static void
create_desktop_window(struct wlr_xdg_surface* xdg_surface)
{
  if (xdg_surface->role == WLR_XDG_SURFACE_ROLE_POPUP) {
    struct wlr_xdg_surface* parent =
      wlr_xdg_surface_from_wlr_surface(xdg_surface->popup->parent);

    // wlroots provides a helper for adding xdg popups to the scene graph, but
    // it requires the popup parent's scene node. For convenience, we always
    // store the scene node in `xdg_surface->data`.
    xdg_surface->data = wlr_scene_xdg_surface_create(parent->data, xdg_surface);
  } else if (xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
    struct desktop_window* window = calloc(1, sizeof(struct desktop_window));

    window->xdg_surface = xdg_surface;
    window->xdg_toplevel = xdg_surface->toplevel;

    window->scene_tree = wlr_scene_xdg_surface_create(
      &server_scene->tree,
      window->xdg_toplevel->base
    );
    window->scene_tree->node.data = window;

    wl_setup_listener(
      &window->map_listener,
      &window->xdg_surface->events.map,
      desktop_window_xdg_surface_map
    );
    wl_setup_listener(
      &window->unmap_listener,
      &window->xdg_surface->events.unmap,
      desktop_window_xdg_surface_unmap
    );
    wl_setup_listener(
      &window->destroy_listener,
      &window->xdg_surface->events.destroy,
      desktop_window_xdg_surface_destroy
    );
    wl_setup_listener(
      &window->request_move_listener,
      &window->xdg_toplevel->events.request_move,
      desktop_window_xdg_toplevel_request_move
    );
    wl_setup_listener(
      &window->request_resize_listener,
      &window->xdg_toplevel->events.request_resize,
      desktop_window_xdg_toplevel_request_resize
    );
    wl_setup_listener(
      &window->request_maximize_listener,
      &window->xdg_toplevel->events.request_maximize,
      desktop_window_xdg_toplevel_request_maximize
    );
    wl_setup_listener(
      &window->request_fullscreen_listener,
      &window->xdg_toplevel->events.request_fullscreen,
      desktop_window_xdg_toplevel_request_fullscreen
    );

    // wlroots provides a helper for adding xdg popups to the scene graph, but
    // it requires the popup parent's scene node. For convenience, we always
    // store the scene node in `xdg_surface->data`.
    xdg_surface->data = window->scene_tree;

    create_lua_desktop_window(window);
  }
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------

static void
server_xdg_shell_new_surface(struct wl_listener* listener, void* data)
{
  struct wlr_xdg_surface* xdg_surface = data;
  create_desktop_window(xdg_surface);
}

void
init_desktop_windows()
{
  wl_setup_listener(
    &server_xdg_shell_new_surface_listener,
    &server_xdg_shell->events.new_surface,
    server_xdg_shell_new_surface
  );
}

// -----------------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------------

int
desktop_window_get_lx(struct desktop_window* window)
{
  int lx, ly;
  wlr_scene_node_coords(&window->scene_tree->node, &lx, &ly);
  return lx;
}

int
desktop_window_get_ly(struct desktop_window* window)
{
  int lx, ly;
  wlr_scene_node_coords(&window->scene_tree->node, &lx, &ly);
  return ly;
}

int
desktop_window_get_gx(struct desktop_window* window)
{
  struct wlr_surface_output* surface_output;
  struct wl_list* surface_outputs =
    &window->xdg_surface->surface->current_outputs;

  wl_list_for_each(surface_output, surface_outputs, link)
  {
    struct wlr_output_layout_output* layout_output =
      wlr_output_layout_get(server_output_layout, surface_output->output);

    if (layout_output != NULL) {
      return layout_output->x + desktop_window_get_lx(window);
    }
  }

  return 0;
}

int
desktop_window_get_gy(struct desktop_window* window)
{
  struct wlr_surface_output* surface_output;
  struct wl_list* surface_outputs =
    &window->xdg_surface->surface->current_outputs;

  wl_list_for_each(surface_output, surface_outputs, link)
  {
    struct wlr_output_layout_output* layout_output =
      wlr_output_layout_get(server_output_layout, surface_output->output);

    if (layout_output != NULL) {
      return layout_output->y + desktop_window_get_ly(window);
    }
  }

  return 0;
}

int
desktop_window_get_width(struct desktop_window* window)
{
  struct wlr_box box;
  wlr_xdg_surface_get_geometry(window->xdg_toplevel->base, &box);
  return box.width;
}

int
desktop_window_get_height(struct desktop_window* window)
{
  struct wlr_box box;
  wlr_xdg_surface_get_geometry(window->xdg_toplevel->base, &box);
  return box.height;
}

bool
desktop_window_get_focused(struct desktop_window* window)
{
  return window->xdg_toplevel->base->surface
         == server_seat->keyboard_state.focused_surface;
}

bool
desktop_window_get_maximized(struct desktop_window* window)
{
  // TODO
  return false;
}

bool
desktop_window_get_fullscreen(struct desktop_window* window)
{
  // TODO
  return false;
}

// -----------------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------------

void
desktop_window_set_lx(struct desktop_window* window, int new_lx)
{
  wlr_scene_node_set_position(
    &window->scene_tree->node,
    new_lx,
    desktop_window_get_ly(window)
  );
}

void
desktop_window_set_ly(struct desktop_window* window, int new_ly)
{
  wlr_scene_node_set_position(
    &window->scene_tree->node,
    desktop_window_get_lx(window),
    new_ly
  );
}

void
desktop_window_set_gx(struct desktop_window* window, int new_gx)
{
  // TODO
}

void
desktop_window_set_gy(struct desktop_window* window, int new_gy)
{
  // TODO
}

void
desktop_window_set_width(struct desktop_window* window, int new_width)
{
  wlr_xdg_toplevel_set_size(
    window->xdg_toplevel,
    new_width,
    desktop_window_get_height(window)
  );
}

void
desktop_window_set_height(struct desktop_window* window, int new_height)
{
  wlr_xdg_toplevel_set_size(
    window->xdg_toplevel,
    desktop_window_get_width(window),
    new_height
  );
}

void
desktop_window_set_focused(struct desktop_window* window, bool new_focused)
{
  if (desktop_window_get_focused(window) == new_focused) {
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
desktop_window_set_maximized(struct desktop_window* window, bool new_maximized)
{
  // TODO
}

void
desktop_window_set_fullscreen(
  struct desktop_window* window,
  bool new_fullscreen
)
{
  // TODO
}

// -----------------------------------------------------------------------------
// Miscellaneous
// -----------------------------------------------------------------------------

struct desktop_window*
get_desktop_window_at(double x, double y, double* nx, double* ny)
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
