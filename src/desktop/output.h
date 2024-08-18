#ifndef CATNIP_DESKTOP_OUTPUT_H
#define CATNIP_DESKTOP_OUTPUT_H

#include <wlr/types/wlr_output.h>

struct catnip_output {
  int id;
  struct wl_list link;
  struct catnip_lua_output* lua_output;

  struct {
    struct wlr_output* output;
    struct wlr_output_layout_output* output_layout_output;
    struct wlr_scene_output* scene_output;
  } wlr;

  struct {
    struct wl_listener output_frame;
    struct wl_listener output_request_state;
    struct wl_listener output_destroy;
  } listeners;
};

struct catnip_output*
catnip_output_create(struct wlr_output* wlr_output);

void
catnip_output_configure(
  struct catnip_output* output,
  int width,
  int height,
  int refresh
);

#endif
