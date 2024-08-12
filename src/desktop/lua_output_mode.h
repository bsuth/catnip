#ifndef CATNIP_DESKTOP_LUA_OUTPUT_MODE_H
#define CATNIP_DESKTOP_LUA_OUTPUT_MODE_H

#include "lua_resource.h"
#include <lua.h>
#include <wlr/types/wlr_output.h>

struct catnip_lua_resource*
lua_catnip_output_mode_create(
  lua_State* L,
  struct wlr_output_mode* output_mode
);

void
lua_catnip_output_mode_destroy(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
);

#endif
