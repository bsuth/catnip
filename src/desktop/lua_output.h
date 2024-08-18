#ifndef CATNIP_DESKTOP_LUA_OUTPUT_H
#define CATNIP_DESKTOP_LUA_OUTPUT_H

#include "desktop/output.h"
#include "extensions/lua.h"

struct catnip_lua_output {
  lua_Ref ref;
  lua_Ref subscriptions;
  struct wl_list link;
  struct catnip_output* output;
  struct catnip_lua_output_modes* lua_output_modes;
};

void
catnip_lua_output_init(lua_State* L);

void
catnip_lua_output_create(lua_State* L, struct catnip_output* output);

void
catnip_lua_output_destroy(lua_State* L, struct catnip_lua_output* lua_output);

void
catnip_lua_output_publish(
  lua_State* L,
  struct catnip_lua_output* lua_output,
  const char* event,
  int nargs
);

#endif
