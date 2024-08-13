#ifndef CATNIP_DESKTOP_LUA_OUTPUTS_H
#define CATNIP_DESKTOP_LUA_OUTPUTS_H

#include "extensions/lua.h"
#include <wayland-util.h>

struct catnip_lua_outputs {
  lua_Ref ref;
  lua_Ref subscriptions;
  struct wl_list outputs;
};

extern struct catnip_lua_outputs* catnip_lua_outputs;

void
catnip_lua_outputs_init(lua_State* L);

void
catnip_lua_outputs_populate(lua_State* L);

#endif
