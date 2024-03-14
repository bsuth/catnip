#ifndef CATNIP_LUA_EVENT_H
#define CATNIP_LUA_EVENT_H

#include "utils/lua.h"
#include <wayland-util.h>

struct catnip_lua_event;

typedef bool (*catnip_lua_event__index)(
  lua_State* L,
  struct catnip_lua_event* lua_event,
  const char* key
);

typedef bool (*catnip_lua_event__newindex)(
  lua_State* L,
  struct catnip_lua_event* lua_event,
  const char* key
);

struct catnip_lua_event {
  struct wl_list link;
  lua_Ref ref;
  void* data;
  catnip_lua_event__index __index;
  catnip_lua_event__newindex __newindex;
};

struct catnip_lua_event*
lua_catnip_event_create(lua_State* L);

void
lua_catnip_event_destroy(lua_State* L, struct catnip_lua_event* lua_event);

void
lua_catnip_event_init(lua_State* L);

#endif
