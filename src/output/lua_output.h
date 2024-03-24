#ifndef CATNIP_LUA_OUTPUT_H
#define CATNIP_LUA_OUTPUT_H

#include "lua_resource.h"
#include "output/output.h"

struct catnip_lua_resource*
lua_catnip_output_create(lua_State* L, struct catnip_output* output);

void
lua_catnip_output_destroy(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
);

void
lua_catnip_output_push_mode(
  lua_State* L,
  struct catnip_output* output,
  struct wlr_output_mode* mode
);

#endif
