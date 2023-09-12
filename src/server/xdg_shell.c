#include "xdg_shell.h"
#include "cursor.h"
#include "display.h"
#include "scene.h"
#include <glib.h>
#include <stdlib.h>

struct wlr_xdg_shell* server_xdg_shell;
struct wl_list server_toplevels;

// -----------------------------------------------------------------------------
// server_toplevel_map
// -----------------------------------------------------------------------------

static void
server_toplevel_map_notify(struct wl_listener* listener, void* data)
{
  struct server_toplevel* view = wl_container_of(listener, view, map);

  wl_list_insert(&server_toplevels, &view->link);

  // TODO focus view
  // focus_view(view, view->xdg_toplevel->base->surface);
}

static void
init_server_toplevel_map(struct server_toplevel* toplevel)
{
  toplevel->map.notify = server_toplevel_map_notify;
  wl_signal_add(&toplevel->xdg_surface->events.map, &toplevel->map);
}

// -----------------------------------------------------------------------------
// server_toplevel_unmap
// -----------------------------------------------------------------------------

static void
server_toplevel_unmap_notify(struct wl_listener* listener, void* data)
{
  struct server_toplevel* toplevel = wl_container_of(listener, toplevel, unmap);

  // TODO
  // Reset the cursor mode if the grabbed view was unmapped.
  // if (toplevel == toplevel->server->grabbed_view) {
  //   reset_cursor_mode(toplevel->server);
  // }

  wl_list_remove(&toplevel->link);
}

static void
init_server_toplevel_unmap(struct server_toplevel* toplevel)
{
  toplevel->unmap.notify = server_toplevel_unmap_notify;
  wl_signal_add(&toplevel->xdg_surface->events.unmap, &toplevel->unmap);
}

// -----------------------------------------------------------------------------
// server_toplevel_destroy
// -----------------------------------------------------------------------------

static void
server_toplevel_destroy_notify(struct wl_listener* listener, void* data)
{
  struct server_toplevel* toplevel =
    wl_container_of(listener, toplevel, destroy);

  wl_list_remove(&toplevel->link);
  wl_list_remove(&toplevel->map.link);
  wl_list_remove(&toplevel->unmap.link);
  wl_list_remove(&toplevel->destroy.link);
  wl_list_remove(&toplevel->request_move.link);
  wl_list_remove(&toplevel->request_resize.link);
  wl_list_remove(&toplevel->request_maximize.link);
  wl_list_remove(&toplevel->request_fullscreen.link);

  free(toplevel);
}

static void
init_server_toplevel_destroy(struct server_toplevel* toplevel)
{
  toplevel->destroy.notify = server_toplevel_destroy_notify;
  wl_signal_add(&toplevel->xdg_surface->events.destroy, &toplevel->destroy);
}

// -----------------------------------------------------------------------------
// server_toplevel_request_move
// -----------------------------------------------------------------------------

static void
server_toplevel_request_move_notify(struct wl_listener* listener, void* data)
{
  // TODO check the provided serial against a list of button press serials sent
  // to this client, to prevent the client from requesting this whenever they
  // want.
  struct server_toplevel* toplevel =
    wl_container_of(listener, toplevel, request_move);
  // TODO start move
}

static void
init_server_toplevel_request_move(struct server_toplevel* toplevel)
{
  toplevel->request_move.notify = server_toplevel_request_move_notify;
  wl_signal_add(
    &toplevel->xdg_toplevel->events.request_move,
    &toplevel->request_move
  );
}

// -----------------------------------------------------------------------------
// server_toplevel_request_resize
// -----------------------------------------------------------------------------

static void
server_toplevel_request_resize_notify(struct wl_listener* listener, void* data)
{
  // TODO check the provided serial against a list of button press serials sent
  // to this client, to prevent the client from requesting this whenever they
  // want.
  struct wlr_xdg_toplevel_resize_event* event = data;
  struct server_toplevel* toplevel =
    wl_container_of(listener, toplevel, request_resize);
  // TODO start resize
}

static void
init_server_toplevel_request_resize(struct server_toplevel* toplevel)
{
  toplevel->request_resize.notify = server_toplevel_request_resize_notify;
  wl_signal_add(
    &toplevel->xdg_toplevel->events.request_resize,
    &toplevel->request_resize
  );
}

// -----------------------------------------------------------------------------
// server_toplevel_request_maximize
// -----------------------------------------------------------------------------

static void
server_toplevel_request_maximize_notify(
  struct wl_listener* listener,
  void* data
)
{
  // TODO support maximization
  struct server_toplevel* toplevel =
    wl_container_of(listener, toplevel, request_maximize);
  wlr_xdg_surface_schedule_configure(toplevel->xdg_toplevel->base);
}

static void
init_server_toplevel_request_maximize(struct server_toplevel* toplevel)
{
  toplevel->request_maximize.notify = server_toplevel_request_maximize_notify;
  wl_signal_add(
    &toplevel->xdg_toplevel->events.request_maximize,
    &toplevel->request_maximize
  );
}

// -----------------------------------------------------------------------------
// server_toplevel_request_fullscreen
// -----------------------------------------------------------------------------

static void
server_toplevel_request_fullscreen_notify(
  struct wl_listener* listener,
  void* data
)
{
  // TODO support fullscreen
  struct server_toplevel* toplevel =
    wl_container_of(listener, toplevel, request_fullscreen);
  wlr_xdg_surface_schedule_configure(toplevel->xdg_toplevel->base);
}

static void
init_server_toplevel_request_fullscreen(struct server_toplevel* toplevel)
{
  toplevel->request_fullscreen.notify =
    server_toplevel_request_fullscreen_notify;
  wl_signal_add(
    &toplevel->xdg_toplevel->events.request_fullscreen,
    &toplevel->request_fullscreen
  );
}

// -----------------------------------------------------------------------------
// xdg_shell_new_surface
// -----------------------------------------------------------------------------

static struct wl_listener xdg_shell_new_surface_listener;

static void
xdg_shell_new_surface_notify(struct wl_listener* listener, void* data)
{
  struct wlr_xdg_surface* xdg_surface = data;

  if (xdg_surface->role == WLR_XDG_SURFACE_ROLE_POPUP) {
    struct wlr_xdg_surface* parent =
      wlr_xdg_surface_from_wlr_surface(xdg_surface->popup->parent);

    // wlroots provides a helper for adding xdg popups to the scene graph, but
    // it requires the popup parent's scene node. For convenience, we always
    // store the scene node in `xdg_surface->data`.
    xdg_surface->data = wlr_scene_xdg_surface_create(parent->data, xdg_surface);
  } else if (xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
    struct server_toplevel* toplevel =
      calloc(1, sizeof(struct server_toplevel));

    toplevel->xdg_surface = xdg_surface;
    toplevel->xdg_toplevel = xdg_surface->toplevel;
    toplevel->scene_tree = wlr_scene_xdg_surface_create(
      &server_scene->tree,
      toplevel->xdg_toplevel->base
    );

    init_server_toplevel_map(toplevel);
    init_server_toplevel_unmap(toplevel);
    init_server_toplevel_destroy(toplevel);
    init_server_toplevel_request_resize(toplevel);
    init_server_toplevel_request_maximize(toplevel);
    init_server_toplevel_request_fullscreen(toplevel);

    // wlroots provides a helper for adding xdg popups to the scene graph, but
    // it requires the popup parent's scene node. For convenience, we always
    // store the scene node in `xdg_surface->data`.
    xdg_surface->data = toplevel->scene_tree;
  }
}

static void
init_xdg_shell_new_surface()
{
  xdg_shell_new_surface_listener.notify = xdg_shell_new_surface_notify;
  wl_signal_add(
    &server_xdg_shell->events.new_surface,
    &xdg_shell_new_surface_listener
  );
};

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------

void
init_server_xdg_shell()
{
  wl_list_init(&server_toplevels);
  server_xdg_shell = wlr_xdg_shell_create(server_display, 3);
  init_xdg_shell_new_surface();
}
