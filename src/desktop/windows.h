#ifndef CATNIP_DESKTOP_WINDOW_H
#define CATNIP_DESKTOP_WINDOW_H

#include "server/xdg_shell.h"

struct desktop_window {
  struct wlr_xdg_surface* xdg_surface;
  struct wlr_xdg_toplevel* xdg_toplevel;
  struct wlr_scene_tree* scene_tree;
  struct wl_listener map_listener;
  struct wl_listener unmap_listener;
  struct wl_listener destroy_listener;
  struct wl_listener request_move_listener;
  struct wl_listener request_resize_listener;
  struct wl_listener request_maximize_listener;
  struct wl_listener request_fullscreen_listener;
};

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------

void
init_desktop_windows();

// -----------------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------------

int
desktop_window_get_lx(struct desktop_window* window);

int
desktop_window_get_ly(struct desktop_window* window);

int
desktop_window_get_gx(struct desktop_window* window);

int
desktop_window_get_gy(struct desktop_window* window);

int
desktop_window_get_width(struct desktop_window* window);

int
desktop_window_get_height(struct desktop_window* window);

bool
desktop_window_get_focused(struct desktop_window* window);

bool
desktop_window_get_maximized(struct desktop_window* window);

bool
desktop_window_get_fullscreen(struct desktop_window* window);

// -----------------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------------

void
desktop_window_set_lx(struct desktop_window* window, int new_lx);

void
desktop_window_set_ly(struct desktop_window* window, int new_ly);

void
desktop_window_set_gx(struct desktop_window* window, int new_gx);

void
desktop_window_set_gy(struct desktop_window* window, int new_gy);

void
desktop_window_set_width(struct desktop_window* window, int new_width);

void
desktop_window_set_height(struct desktop_window* window, int new_height);

void
desktop_window_set_focused(struct desktop_window* window, bool new_focused);

void
desktop_window_set_maximized(struct desktop_window* window, bool new_maximized);

void
desktop_window_set_fullscreen(
  struct desktop_window* window,
  bool new_fullscreen
);

// -----------------------------------------------------------------------------
// Miscellaneous
// -----------------------------------------------------------------------------

struct desktop_window*
get_desktop_window_at(double x, double y, double* nx, double* ny);

#endif
