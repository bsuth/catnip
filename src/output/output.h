#ifndef CATNIP_OUTPUT_H
#define CATNIP_OUTPUT_H

#include "utils/lua.h"
#include <wlr/types/wlr_output.h>

struct catnip_output {
  struct wl_list link;
  struct wlr_output* wlr_output;

  struct {
    struct catnip_output** userdata;
    lua_Ref modes;
  } lua;

  struct {
    struct wl_listener frame;
    struct wl_listener destroy;
  } listeners;
};

extern struct wl_list catnip_outputs;

void
catnip_output_init();

#endif
