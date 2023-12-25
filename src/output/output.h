#ifndef CATNIP_OUTPUT_H
#define CATNIP_OUTPUT_H

#include "utils/lua.h"
#include <wlr/types/wlr_output.h>

struct catnip_output {
  int id;
  struct wl_list link;

  struct wlr_output* wlr_output;
  struct wlr_output_layout_output* layout_output;
  struct wlr_scene_output* scene_output;

  struct {
    struct wl_listener frame;
    struct wl_listener request_state;
    struct wl_listener destroy;
  } listeners;

  struct {
    struct catnip_output** userdata;
    lua_Ref ref;
    lua_Ref subscriptions;
    lua_Ref modes;
  } lua;
};

extern struct wl_list catnip_outputs;

void
catnip_output_init();

#endif
