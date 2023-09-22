#ifndef CATNIP_DESKTOP_MONITOR_H
#define CATNIP_DESKTOP_MONITOR_H

#include <wlr/types/wlr_output.h>

struct desktop_monitor {
  struct wlr_output* output;
  struct wl_listener frame;
  struct wl_listener destroy;
};

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------

void
init_desktop_monitors();

// -----------------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------------

int
desktop_monitor_get_x(struct desktop_monitor* monitor);

int
desktop_monitor_get_y(struct desktop_monitor* monitor);

int
desktop_monitor_get_width(struct desktop_monitor* monitor);

int
desktop_monitor_get_height(struct desktop_monitor* monitor);

int
desktop_monitor_get_refresh(struct desktop_monitor* monitor);

struct wlr_output_mode*
desktop_monitor_get_mode(struct desktop_monitor* monitor);

struct wl_list
desktop_monitor_get_modes(struct desktop_monitor* monitor);

float
desktop_monitor_get_scale(struct desktop_monitor* monitor);

// -----------------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------------

void
desktop_monitor_set_x(struct desktop_monitor* monitor, int new_x);

void
desktop_monitor_set_y(struct desktop_monitor* monitor, int new_y);

void
desktop_monitor_set_width(struct desktop_monitor* monitor, int new_width);

void
desktop_monitor_set_height(struct desktop_monitor* monitor, int new_height);

void
desktop_monitor_set_refresh(struct desktop_monitor* monitor, int new_refresh);

void
desktop_monitor_set_mode(
  struct desktop_monitor* monitor,
  struct wlr_output_mode* new_mode
);

void
desktop_monitor_set_scale(struct desktop_monitor* monitor, float new_scale);

#endif
