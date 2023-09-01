#include "xdg_shell.h"
#include "cursor.h"
#include "display.h"
#include "scene.h"
#include <stdlib.h>

struct wlr_xdg_shell* server_xdg_shell;
struct wl_list server_views;

static struct wl_listener xdg_shell_new_surface_listener;

static void
begin_interactive(struct server_view* view,
                  enum server_cursor_mode mode,
                  uint32_t edges)
{
  // TODO
  /*   struct wlr_surface* focused_surface = */
  /*     server->seat->pointer_state.focused_surface; */

  /*   if (view->xdg_toplevel->base->surface != */
  /*       wlr_surface_get_root_surface(focused_surface)) { */
  /*     /1* Deny move/resize requests from unfocused clients. *1/ */
  /*     return; */
  /*   } */
  /*   server->grabbed_view = view; */
  /*   server->cursor_mode = mode; */

  /*   if (mode == TINYWL_CURSOR_MOVE) { */
  /*     server->grab_x = server->cursor->x - view->x; */
  /*     server->grab_y = server->cursor->y - view->y; */
  /*   } else { */
  /*     struct wlr_box geo_box; */
  /*     wlr_xdg_surface_get_geometry(view->xdg_toplevel->base, &geo_box); */

  /*     double border_x = */
  /*       (view->x + geo_box.x) + ((edges & WLR_EDGE_RIGHT) ? geo_box.width :
   * 0);
   */
  /*     double border_y = */
  /*       (view->y + geo_box.y) + ((edges & WLR_EDGE_BOTTOM) ? geo_box.height :
   * 0); */
  /*     server->grab_x = server->cursor->x - border_x; */
  /*     server->grab_y = server->cursor->y - border_y; */

  /*     server->grab_geobox = geo_box; */
  /*     server->grab_geobox.x += view->x; */
  /*     server->grab_geobox.y += view->y; */

  /*     server->resize_edges = edges; */
  /*   } */
}

static void
xdg_toplevel_map(struct wl_listener* listener, void* data)
{
  struct server_view* view = wl_container_of(listener, view, map);

  wl_list_insert(&server_views, &view->link);

  /* focus_view(view, view->xdg_toplevel->base->surface); */
}

static void
xdg_toplevel_unmap(struct wl_listener* listener, void* data)
{
  struct server_view* view = wl_container_of(listener, view, unmap);

  /* Reset the cursor mode if the grabbed view was unmapped. */
  /* if (view == view->server->grabbed_view) { */
  /*   reset_cursor_mode(view->server); */
  /* } */

  wl_list_remove(&view->link);
}

static void
xdg_toplevel_destroy(struct wl_listener* listener, void* data)
{
  struct server_view* view = wl_container_of(listener, view, destroy);

  wl_list_remove(&view->map.link);
  wl_list_remove(&view->unmap.link);
  wl_list_remove(&view->destroy.link);
  wl_list_remove(&view->request_move.link);
  wl_list_remove(&view->request_resize.link);
  wl_list_remove(&view->request_maximize.link);
  wl_list_remove(&view->request_fullscreen.link);

  free(view);
}

static void
xdg_toplevel_request_move(struct wl_listener* listener, void* data)
{
  // TODO check the provided serial against a list of button press serials sent
  // to this client, to prevent the client from requesting this whenever they
  // want.
  struct server_view* view = wl_container_of(listener, view, request_move);
  begin_interactive(view, SERVER_CURSOR_MODE_MOVE, 0);
}

static void
xdg_toplevel_request_resize(struct wl_listener* listener, void* data)
{
  // TODO check the provided serial against a list of button press serials sent
  // to this client, to prevent the client from requesting this whenever they
  // want.
  struct wlr_xdg_toplevel_resize_event* event = data;
  struct server_view* view = wl_container_of(listener, view, request_resize);
  begin_interactive(view, SERVER_CURSOR_MODE_RESIZE, event->edges);
}

static void
xdg_toplevel_request_maximize(struct wl_listener* listener, void* data)
{
  // TODO support maximization
  struct server_view* view = wl_container_of(listener, view, request_maximize);
  wlr_xdg_surface_schedule_configure(view->xdg_toplevel->base);
}

static void
xdg_toplevel_request_fullscreen(struct wl_listener* listener, void* data)
{
  // TODO support fullscreen
  struct server_view* view =
    wl_container_of(listener, view, request_fullscreen);
  wlr_xdg_surface_schedule_configure(view->xdg_toplevel->base);
}

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
    struct server_view* view = calloc(1, sizeof(struct server_view));

    view->xdg_toplevel = xdg_surface->toplevel;
    view->scene_tree = wlr_scene_xdg_surface_create(&server_scene->tree,
                                                    view->xdg_toplevel->base);

    // wlroots provides a helper for adding xdg popups to the scene graph, but
    // it requires the popup parent's scene node. For convenience, we always
    // store the scene node in `xdg_surface->data`.
    xdg_surface->data = view->scene_tree;

    view->map.notify = xdg_toplevel_map;
    wl_signal_add(&xdg_surface->events.map, &view->map);
    view->unmap.notify = xdg_toplevel_unmap;
    wl_signal_add(&xdg_surface->events.unmap, &view->unmap);
    view->destroy.notify = xdg_toplevel_destroy;
    wl_signal_add(&xdg_surface->events.destroy, &view->destroy);

    struct wlr_xdg_toplevel* toplevel = xdg_surface->toplevel;

    view->request_move.notify = xdg_toplevel_request_move;
    wl_signal_add(&toplevel->events.request_move, &view->request_move);
    view->request_resize.notify = xdg_toplevel_request_resize;
    wl_signal_add(&toplevel->events.request_resize, &view->request_resize);
    view->request_maximize.notify = xdg_toplevel_request_maximize;
    wl_signal_add(&toplevel->events.request_maximize, &view->request_maximize);
    view->request_fullscreen.notify = xdg_toplevel_request_fullscreen;
    wl_signal_add(&toplevel->events.request_fullscreen,
                  &view->request_fullscreen);
  }
}

void
server_xdg_shell_init()
{
  wl_list_init(&server_views);
  server_xdg_shell = wlr_xdg_shell_create(server_display, 3);
  xdg_shell_new_surface_listener.notify = xdg_shell_new_surface_notify;
  wl_signal_add(&server_xdg_shell->events.new_surface,
                &xdg_shell_new_surface_listener);
}
