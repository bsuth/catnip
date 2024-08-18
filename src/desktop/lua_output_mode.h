#ifndef CATNIP_DESKTOP_LUA_OUTPUT_MODE_H
#define CATNIP_DESKTOP_LUA_OUTPUT_MODE_H

#include "extensions/lua.h"
#include <wlr/types/wlr_output.h>

struct catnip_lua_output_mode {
  lua_Ref ref;
  struct wl_list link;
  struct wlr_output_mode* wlr_output_mode;
};

void
catnip_lua_output_mode_init(lua_State* L);

struct catnip_lua_output_mode*
catnip_lua_output_mode_create(
  lua_State* L,
  struct wlr_output_mode* wlr_output_mode
);

void
catnip_lua_output_mode_destroy(
  lua_State* L,
  struct catnip_lua_output_mode* lua_output_mode
);

#endif
