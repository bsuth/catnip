#ifndef CATNIP_DESKTOP_LUA_OUTPUT_H
#define CATNIP_DESKTOP_LUA_OUTPUT_H

#include "desktop/output.h"
#include "lua_resource.h"

struct catnip_lua_resource*
lua_catnip_output_create(lua_State* L, struct catnip_output* output);

void
lua_catnip_output_destroy(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
);

#endif
