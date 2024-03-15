#ifndef CATNIP_WINDOW_H
#define CATNIP_WINDOW_H

#include "lua_resource.h"
#include <wlr/types/wlr_xdg_shell.h>

struct catnip_window {
  struct wl_list link;
  struct catnip_lua_resource* lua_resource;

  struct wlr_xdg_surface* xdg_surface;
  struct wlr_xdg_toplevel* xdg_toplevel;
  struct wlr_scene_tree* scene_tree;

  struct {
    struct wl_listener map;
    struct wl_listener unmap;
    struct wl_listener configure;
    struct wl_listener request_move;
    struct wl_listener request_resize;
    struct wl_listener request_maximize;
    struct wl_listener request_fullscreen;
    struct wl_listener destroy;
  } listeners;

  // Since properties that are mutated through `configure` events are not
  // updated immediately (but rather on the next event cycle), we store the
  // latest known values here.
  //
  // This struct is also used to do diffs so we know whether or not we need to
  // publish a `property::xxx` event (since the `configure` event may be
  // triggered by a third party and does not tell us which fields have changed).
  //
  // Note that we intentionally do not use `xdg_toplevel->scheduled`, since if
  // this is changed by a third party, we don't have a chance to emit a
  // `property::xxx` event until the next `configure` event, but the Lua getter
  // will fetch the (secretly) changed value immediately.
  struct {
    int32_t width;
    int32_t height;
    bool resizing;

    // Even though we sync the `activated` state with whether or not the window
    // is "focused", we keep separate properties here since they serve different
    // functions.
    //
    // `focused` is used to track the property value itself and is used when
    // determining whether we need to publish a `property::focused` event.
    //
    // `activated` is used to track whether or we need to sync the keyboard's
    // focused surface state.
    bool activated;
    bool focused;
  } latest;
};

extern struct wl_list catnip_windows;

void
catnip_window_init();

#endif
