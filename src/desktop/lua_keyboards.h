#ifndef CATNIP_DESKTOP_LUA_KEYBOARDS_H
#define CATNIP_DESKTOP_LUA_KEYBOARDS_H

#include "extensions/lua.h"
#include <wayland-util.h>

struct catnip_lua_keyboards {
  lua_Ref ref;
  lua_Ref subscriptions;
  struct wl_list keyboards;
};

extern struct catnip_lua_keyboards* catnip_lua_keyboards;

void
catnip_lua_keyboards_init(lua_State* L);

void
catnip_lua_keyboards_populate(lua_State* L);

#endif
