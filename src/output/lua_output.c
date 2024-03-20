#include "lua_output.h"
#include "lua_resource.h"
#include "output/lua_output_list.h"
#include "output/lua_output_mode_list.h"
#include "output/output_properties.h"
#include "utils/string.h"
#include <lauxlib.h>

static void
lua_catnip_output_push_current_mode(lua_State* L, struct catnip_output* output)
{
  struct wlr_output_mode* current_mode = catnip_output_get_mode(output);

  struct catnip_lua_resource* lua_resource = NULL;
  wl_list_for_each(lua_resource, &output->lua_mode_list->head, link)
  {
    if (lua_resource->data == current_mode) {
      lua_rawgeti(L, LUA_REGISTRYINDEX, lua_resource->ref);
      return;
    }
  }

  lua_pushnil(L);
}

static bool
lua_catnip_output__index(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  struct catnip_output* output = lua_resource->data;

  if (streq(key, "x")) {
    lua_pushnumber(L, catnip_output_get_x(output));
  } else if (streq(key, "y")) {
    lua_pushnumber(L, catnip_output_get_y(output));
  } else if (streq(key, "width")) {
    lua_pushnumber(L, catnip_output_get_width(output));
  } else if (streq(key, "height")) {
    lua_pushnumber(L, catnip_output_get_height(output));
  } else if (streq(key, "refresh")) {
    lua_pushnumber(L, catnip_output_get_refresh(output));
  } else if (streq(key, "mode")) {
    lua_catnip_output_push_current_mode(L, output);
  } else if (streq(key, "modes")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, output->lua_mode_list->ref);
  } else if (streq(key, "scale")) {
    lua_pushnumber(L, catnip_output_get_scale(output));
  } else {
    return false;
  }

  return true;
}

static bool
lua_catnip_output__newindex(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  struct catnip_output* output = lua_resource->data;

  if (streq(key, "x")) {
    catnip_output_set_x(output, luaL_checknumber(L, 3));
  } else if (streq(key, "y")) {
    catnip_output_set_y(output, luaL_checknumber(L, 3));
  } else if (streq(key, "width")) {
    catnip_output_set_width(output, luaL_checknumber(L, 3));
  } else if (streq(key, "height")) {
    catnip_output_set_height(output, luaL_checknumber(L, 3));
  } else if (streq(key, "refresh")) {
    catnip_output_set_refresh(output, luaL_checknumber(L, 3));
  } else if (streq(key, "mode")) {
    struct wlr_output_mode** lua_output_mode =
      luaL_checkudata(L, 3, "catnip.output.mode");
    catnip_output_set_mode(output, *lua_output_mode);
  } else if (streq(key, "scale")) {
    catnip_output_set_scale(output, luaL_checknumber(L, 3));
  } else {
    return false;
  }

  return true;
}

struct catnip_lua_resource*
lua_catnip_output_create(lua_State* L, struct catnip_output* output)
{
  struct catnip_lua_resource* lua_resource = lua_catnip_resource_create(L);
  output->lua_resource = lua_resource;
  output->lua_mode_list = lua_catnip_output_mode_list_create(L, output);

  lua_resource->data = output;
  lua_resource->name = "output";
  lua_resource->__index = lua_catnip_output__index;
  lua_resource->__newindex = lua_catnip_output__newindex;

  wl_list_insert(&lua_catnip_output_list->head, &lua_resource->link);

  lua_catnip_resource_publish(L, lua_resource, "create", 0);

  return lua_resource;
}

void
lua_catnip_output_destroy(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
)
{
  struct catnip_output* output = lua_resource->data;
  lua_catnip_resource_publish(L, lua_resource, "destroy", 0);
  lua_catnip_resource_list_destroy(L, output->lua_mode_list);
  lua_catnip_resource_destroy(L, lua_resource);
}
