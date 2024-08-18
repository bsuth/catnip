#ifndef CATNIP_DESKTOP_LUA_WINDOWS_H
#define CATNIP_DESKTOP_LUA_WINDOWS_H

#include "extensions/lua.h"
#include <wayland-util.h>

struct catnip_lua_windows {
  lua_Ref ref;
  lua_Ref subscriptions;
  struct wl_list windows;
};

extern struct catnip_lua_windows* catnip_lua_windows;

void
catnip_lua_windows_init(lua_State* L);

void
catnip_lua_windows_populate(lua_State* L);

#endif
