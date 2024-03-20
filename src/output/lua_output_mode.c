#include "lua_output_mode.h"
#include "utils/string.h"
#include <lauxlib.h>

static bool
lua_catnip_output_mode__index(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  struct wlr_output_mode* mode = lua_resource->data;

  if (streq(key, "width")) {
    lua_pushnumber(L, mode->width);
  } else if (streq(key, "height")) {
    lua_pushnumber(L, mode->height);
  } else if (streq(key, "refresh")) {
    lua_pushnumber(L, mode->refresh);
  } else {
    return false;
  }

  return true;
}

struct catnip_lua_resource*
lua_catnip_output_mode_create(lua_State* L, struct wlr_output_mode* output_mode)
{
  struct catnip_lua_resource* lua_resource = lua_catnip_resource_create(L);

  lua_resource->data = output_mode;
  lua_resource->__index = lua_catnip_output_mode__index;

  return lua_resource;
}

void
lua_catnip_output_mode_destroy(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
)
{
  lua_catnip_resource_destroy(L, lua_resource);
}
