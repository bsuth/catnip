#ifndef CATNIP_SERVER_WINDOW_H
#define CATNIP_SERVER_WINDOW_H

#include "server/xdg_shell.h"

struct server_window {
  struct wl_list link;
  struct wlr_xdg_surface* xdg_surface;
  struct wlr_xdg_toplevel* xdg_toplevel;
  struct wlr_scene_tree* scene_tree;

  struct {
    struct wl_listener map;
    struct wl_listener unmap;
    struct wl_listener destroy;
    struct wl_listener request_move;
    struct wl_listener request_resize;
    struct wl_listener request_maximize;
    struct wl_listener request_fullscreen;
  } listeners;

  struct {
    struct wl_signal destroy;
  } events;

  // These properties cannot be updated immediately and must instead _request_
  // an update. Here, we store the most recent values requested.
  struct {
    int width;
    int height;
  } pending;
};

struct server_window_events {
  struct wl_signal new_server_window;
};

extern struct wl_list server_windows;
extern struct server_window_events server_window_events;

// -----------------------------------------------------------------------------
// Server Window
// -----------------------------------------------------------------------------

struct server_window*
server_window_get_at(double x, double y, double* nx, double* ny);

// -----------------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------------

int
server_window_get_x(struct server_window* window);

int
server_window_get_y(struct server_window* window);

int
server_window_get_width(struct server_window* window);

int
server_window_get_height(struct server_window* window);

bool
server_window_get_focused(struct server_window* window);

bool
server_window_get_maximized(struct server_window* window);

bool
server_window_get_fullscreen(struct server_window* window);

// -----------------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------------

void
server_window_set_x(struct server_window* window, int new_x);

void
server_window_set_y(struct server_window* window, int new_y);

void
server_window_set_width(struct server_window* window, int new_width);

void
server_window_set_height(struct server_window* window, int new_height);

void
server_window_set_focused(struct server_window* window, bool new_focused);

void
server_window_set_maximized(struct server_window* window, bool new_maximized);

void
server_window_set_fullscreen(struct server_window* window, bool new_fullscreen);

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

void
server_window_init();

#endif
