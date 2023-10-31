#ifndef CATNIP_SERVER_OUTPUT_H
#define CATNIP_SERVER_OUTPUT_H

#include <wlr/types/wlr_output.h>

struct server_output {
  struct wl_list link;
  struct wlr_output* wlr_output;

  struct {
    struct wl_listener frame;
    struct wl_listener destroy;
  } listeners;

  struct {
    struct wl_signal destroy;
  } events;
};

struct server_output_events {
  struct wl_signal new_server_output;
};

extern struct wl_list server_outputs;
extern struct server_output_events server_output_events;

// -----------------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------------

int
server_output_get_x(struct server_output* output);

int
server_output_get_y(struct server_output* output);

int
server_output_get_width(struct server_output* output);

int
server_output_get_height(struct server_output* output);

int
server_output_get_refresh(struct server_output* output);

struct wlr_output_mode*
server_output_get_mode(struct server_output* output);

struct wl_list*
server_output_get_modes(struct server_output* output);

float
server_output_get_scale(struct server_output* output);

// -----------------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------------

void
server_output_set_x(struct server_output* output, int new_x);

void
server_output_set_y(struct server_output* output, int new_y);

void
server_output_set_width(struct server_output* output, int new_width);

void
server_output_set_height(struct server_output* output, int new_height);

void
server_output_set_refresh(struct server_output* output, int new_refresh);

void
server_output_set_mode(
  struct server_output* output,
  struct wlr_output_mode* new_mode
);

void
server_output_set_scale(struct server_output* output, float new_scale);

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

void
server_output_init();

#endif
