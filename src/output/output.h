#ifndef CATNIP_OUTPUT_H
#define CATNIP_OUTPUT_H

#include "lua_resource.h"
#include "lua_resource_list.h"
#include <wlr/types/wlr_output.h>

struct catnip_output {
  int id;

  struct wl_list link;
  struct catnip_lua_resource* lua_resource;
  struct catnip_lua_resource_list* lua_mode_list;

  struct wlr_output* wlr_output;
  struct wlr_output_layout_output* layout_output;
  struct wlr_scene_output* scene_output;

  struct {
    struct wl_listener frame;
    struct wl_listener request_state;
    struct wl_listener destroy;
  } listeners;
};

extern struct wl_list catnip_outputs;

void
catnip_output_init();

#endif
