#ifndef CATNIP_DESKTOP_LUA_OUTPUT_MODES_H
#define CATNIP_DESKTOP_LUA_OUTPUT_MODES_H

#include "desktop/output.h"
#include "extensions/lua.h"
#include <wlr/types/wlr_output.h>

struct catnip_lua_output_modes {
  lua_Ref ref;
  struct wl_list modes;
};

void
catnip_lua_output_modes_init(lua_State* L);

struct catnip_lua_output_modes*
catnip_lua_output_modes_create(lua_State* L, struct catnip_output* output);

void
catnip_lua_output_modes_destroy(
  lua_State* L,
  struct catnip_lua_output_modes* lua_output_modes
);

#endif
